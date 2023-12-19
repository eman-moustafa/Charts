#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include "data.h"
#include "settingsdialog.h"
#include<QtCharts>
#include <QChartView>
#include <QLineSeries>
#include <QValueAxis>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    QChartView *chartView;
    QLineSeries *series;
    QLineSeries *series2;
    QLineSeries *series3;
    QLineSeries *series4;
    QLineSeries *series5;
    QValueAxis *axisX;
    QValueAxis *axisY;
    QTimer *timer;

private slots:
    void updateChart();
     void createChart();


private slots:
    void openSerialPort();
    void writeData(const QByteArray &data);
    void readData();
    QVector<QString>  strokeData(QString Data);
    void parseData(QVector<QString>);

private:
    Ui::MainWindow *m_ui;
     QSerialPort *m_serial = nullptr;
      QVector<QString> LastFrame;
      Data *m_Data;
      SettingsDialog *m_settings = nullptr;
};
#endif // MAINWINDOW_H
