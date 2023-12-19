#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QDebug"
#include <QMovie>
#include <QInputDialog>
#include <QTimer>
#include <QToolTip>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow),
      m_serial(new QSerialPort(this)),
      m_Data(new Data),
      m_settings(new SettingsDialog)
{
    m_ui->setupUi(this);
 m_ui->actionConnect->setEnabled(true);
 m_ui->actionConfigure->setEnabled(true);


qDebug()<<"123";
    connect(m_serial, &QSerialPort::readyRead, this, &MainWindow::readData);
connect(m_ui->actionConnect, &QAction::triggered, this, &MainWindow::openSerialPort);
connect(m_ui->actionConfigure, &QAction::triggered, m_settings, &SettingsDialog::showSettings);

createChart();

}

void MainWindow::updateChart()
{
    // Generate random data for the line series
    int x = QDateTime::currentDateTime().toMSecsSinceEpoch() / 1000.0; // Current timestamp


    series->append(x, m_Data->SpeedFeedBack.toDouble());
    series2->append(x, m_Data->PhaseData.toDouble());
    series3->append(x, m_Data->FD.toDouble());
    series4->append(x, m_Data->H_l_CurrentAngle.toDouble());
    series5->append(x, m_Data->LcdTemp.toDouble());

    // Adjust the X axis range to show the last N seconds of data
    double minX = x - 10.0; // Show the last 10 seconds
    double maxX = x;
    axisX->setRange(minX, maxX);


    // Adjust the Y axis range based on the data
    double minY = series5->at(0).y();
    double maxY = series5->at(0).y();
    for (const QPointF &point : series5->points()) {
        if (point.y() < minY)
            minY = point.y();
        if (point.y() > maxY)
            maxY = point.y();
    }
    axisY->setRange(minY, maxY);

}



void MainWindow::createChart()
{
    // Create the line series
      series = new QLineSeries();
series2 = new QLineSeries();
series3= new QLineSeries();
series4 = new QLineSeries();
series5 = new QLineSeries();
      // Create the chart and set the series
      QChart *chart = new QChart();
      chart->addSeries(series);
      chart->addSeries(series2);
      chart->addSeries(series3);
      chart->addSeries(series4);
      chart->addSeries(series5);

// Create the X and Y axes
      axisX = new QValueAxis();
      axisY = new QValueAxis();

      chart->addAxis(axisX, Qt::AlignBottom);
      chart->addAxis(axisY, Qt::AlignLeft);

      series->attachAxis(axisX);
      series->attachAxis(axisY);

      series2->attachAxis(axisX);
      series2->attachAxis(axisY);

      series3->attachAxis(axisX);
      series3->attachAxis(axisY);

      series4->attachAxis(axisX);
      series4->attachAxis(axisY);

      series5->attachAxis(axisX);
      series5->attachAxis(axisY);

      // Create the chart view
      chartView = new QChartView(chart);
      setCentralWidget(chartView);

      // Set up the timer for periodic updates
      timer = new QTimer(this);
      connect(timer, SIGNAL(timeout()), this, SLOT(updateChart()));
      timer->start(1000); // Update every 1 second
}

MainWindow::~MainWindow()
{
    delete m_ui;
}


void MainWindow::openSerialPort()
{
    const SettingsDialog::Settings p = m_settings->settings();
    m_serial->setPortName(p.name);
    m_serial->setBaudRate(p.baudRate);
    m_serial->setDataBits(p.dataBits);
    m_serial->setParity(p.parity);
    m_serial->setStopBits(p.stopBits);
    m_serial->setFlowControl(p.flowControl);
    if (m_serial->open(QIODevice::ReadWrite)) {
        m_ui->actionConnect->setEnabled(true);
        m_ui->actionConfigure->setEnabled(false);


    }
    else
        qDebug()<<"Error";
}
void MainWindow::writeData(const QByteArray &data)
{
    m_serial->write(data);
}
//! [6]

//! [7]
void MainWindow::readData()
{

    if(m_serial->canReadLine()){
        qDebug()<<"open"<<endl;

        const QByteArray data = m_serial->readLine();
        qDebug()<<data<<endl;
//        qDebug()<<strokeData(QString(data).trimmed());
        parseData(strokeData(QString(data).trimmed()));
}

}

QVector<QString> MainWindow::strokeData(QString Data)
{
    QVector<QString> res;
        QString delim = ",";
        QString token = "";
        bool flag;
        for (int i = 0; i < Data.size(); i++) {
             flag = true;
            if(Data[i]!=delim)flag = false;
            if (flag) {
                if (token.size() > 0) {
                    res.push_back(token);
                    token = "";
                }
            } else {
                token += Data[i];
            }
        }
        res.push_back(token);
        return res;

}

void MainWindow::parseData(QVector<QString> Data)
{
    LastFrame=Data;
    QVector<QString>::iterator itt;
            for (itt = Data.begin(); itt != Data.end(); ++itt) {
                QStringList list = (*itt).split(QRegExp("\\s+"), QString::SkipEmptyParts);
                if(list.length()>=2){
                    //Monitor Data
                    if(list[0]=="A"){
                        m_Data->setAbility(list[1]);
                        m_Data->setMonitorabilityVersion(list[1]);
                    }

                    else if (list[0]=="MM") m_Data->setMonitorModel(list[1]);
                    else if (list[0]=="FV") m_Data->setFlashVersion(list[1]);
                    else if (list[0]=="EV") {
                                            m_Data->setEEPVersion(list[1]);
                                            m_Data->setModelEEP(list[1]);
                    }
                    else if (list[0]=="MP") m_Data->setMotorPartCode(list[1]);
                    else if (list[0]=="MT") m_Data->setMotorType(list[1]);

                    //operation states
                    else if (list[0]=="OC") m_Data->setOperationCycle(list[1]);
                    else if (list[0]=="SP") m_Data->setSpMeasur(list[1]);
                    else if (list[0]=="FD") {
                                            m_Data->setFD(list[1]);
                    }
                    else if (list[0]=="M")  m_Data->setOperationMode(list[1]);
                    else if (list[0]=="FC") m_Data->setoutFanControl(list[1]);
                    else if (list[0]=="MV") m_Data->setACModel(list[1]);
                    else if (list[0]=="EC") m_Data->setErrorCode(list[1]);
                    else if (list[0]=="PM") m_Data->setModeMemory(list[1]);
                    else if (list[0]=="DL") m_Data->setDehumiditylifeMode(list[1]);
                    else if (list[0]=="DA") m_Data->setDehumidityAuto(list[1]);
                    else if (list[0]=="AM") m_Data->setAutoMask(list[1]);
                    else if (list[0]=="PT") m_Data->setOperationTime(list[1]);
                    else if (list[0]=="PF") m_Data->setOperationState(list[1]);
                    else if (list[0]=="HK") m_Data->setHotKeep(list[1]);
                    else if (list[0]=="DS") m_Data->setDehumidityState(list[1]);
                    else if (list[0]=="D")  m_Data->setDehumidityTime(list[1]);
                    else if (list[0]=="HD")  m_Data->setLcdState(list[1]);
                    else if (list[0]=="CT") {
                                            m_Data->setLcdTemp(list[1]);
                                            m_Data->setDisplaySettingsTemp(list[1]);
                                            m_Data->setCorrectedRoomTemp(list[1]);
                    }
                    else if (list[0]=="BO") m_Data->setBadodorTimer(list[1]);
                    else if (list[0]=="FT") m_Data->setFilterTimer(list[1]);

                    //indoor Settings
                    else if (list[0]=="OS") m_Data->setOutDoorTemp(list[1]);
                    else if (list[0]=="ST"){
                                            m_Data->setSettingTemp(list[1]);
                                            m_Data->setRemoconSet(list[1]);
                    }
                    else if (list[0]=="C")  m_Data->setCompressorState(list[1]);
                    else if (list[0]=="CN") m_Data->setCompressorOnTime(list[1]);
                    else if (list[0]=="CF") m_Data->setCompressorOFFTime(list[1]);
                    else if (list[0]=="STC") m_Data->setSafetyTimerControl(list[1]);
                    else if (list[0]=="BT") m_Data->setBelowTemp(list[1]);
                    else if (list[0]=="VT") m_Data->setValveTemp(list[1]);
                    else if (list[0]=="RT") {
                                            m_Data->setTempNow(list[1]);
                                            m_Data->setIdRoomSensorTemp(list[1]);
                    }
                    else if (list[0]=="ET") {
                                             m_Data->setIDPipeTemp(list[1]);
                    }
                    else if (list[0]=="NI")  m_Data->setInverter_Normal(list[1]);
                    else if (list[0]=="PBT") m_Data->setBelowTemp_Before(list[1]);
                    else if (list[0]=="PVT") m_Data->setValveTemp_Before(list[1]);
                    else if (list[0]=="RTB") m_Data->setRoomTemp_Before(list[1]);
                    else if (list[0]=="ETB") m_Data->setIDpipeTemp_Before(list[1]);
                    else if (list[0]=="DM")  m_Data->setIDpipeMinWhileDefrost_Before(list[1]);
                    else if (list[0]=="M1")  m_Data->setMAinOff_T1(list[1]);
                    else if (list[0]=="M2")  m_Data->setMAinOff_T2(list[1]);


                    //outdoor fan
                    else if (list[0]=="OF")  m_Data->setOutDoorFanState(list[1]);
                    else if (list[0]=="OFS") m_Data->setOutDoorFanSpeed(list[1]);
                    //heater
                    else if (list[0]=="HS")  m_Data->setHeaterState(list[1]);
                    //Fan
                    else if (list[0]=="F")   m_Data->setFanSpeed(list[1]);
                    else if (list[0]=="TS")  m_Data->setTargetSpeed(list[1]);
                    else if (list[0]=="FB")  m_Data->setFanSpeedFeedBack(list[1]);
                    else if (list[0]=="D1")  m_Data->setFanDuty(list[1]);
                    else if (list[0]=="PD")  m_Data->setPhaseData(list[1]);
                    else if (list[0]=="D2")  m_Data->setFanOnDelay(list[1]);
                    else if (list[0]=="DT")  m_Data->setDryFanOnOffTimer(list[1]);
                    //A/D

                    else if (list[0]=="HMS") m_Data->setHumiditySensor(list[1]);
                    else if (list[0]=="MZ")  m_Data->setIDRoomZone(list[1]);
                    else if (list[0]=="HZ")  m_Data->setIDPipeZone(list[1]);
                    else if (list[0]=="HMZ") m_Data->setHumidityZone(list[1]);
                    else if (list[0]=="CRT") m_Data->setControlTemp(list[1]);

                    //H-L Louver
                    else if (list[0]=="HLP") {
                                             m_Data->setH_l_CurrentAngle(list[1]);
                    }
                    else if (list[0]=="HL")  m_Data->setH_l_StateName(list[1]);
                    else if (list[0]=="HM")  m_Data->setH_l_MemoryAngle(list[1]);

                    //V-L louver
                    else if (list[0]=="VLP") {
                                             m_Data->setV_l_CurrentAngle(list[1]);
                    }
                    else if (list[0]=="VL")  m_Data->setV_l_StateName(list[1]);
                    else if (list[0]=="VM")  m_Data->setV_l_MemoryAngle(list[1]);
                    else if (list[0]=="Time")  m_Data->setTime(list[1]);

                    else
                        qDebug()<<"Undefined Key Frame : "<<list[0]<<endl;



            }
            }

}


