#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QDialog>
#include <QTextEdit>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QResizeEvent>
#include <QGraphicsEllipseItem>

#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QValueAxis>

#include <unordered_map>
#include <fstream>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow *ui;
    std::ifstream sensorFile;
    QTimer *timer;
    QGraphicsScene *planeScene;
    QGraphicsPixmapItem *planeItem;

    void setupPlane();
    void fitPlaneToView();
    void startSensorData();
    void readNextSensor();
    void toggleSensorData();

    std::unordered_map<std::string, QGraphicsEllipseItem*> sensorMarkers;

    QChart *voltageChart;
    QChartView *voltageChartView;
    QLineSeries *voltageSeries;

    QValueAxis *voltageAxisX;
    QValueAxis *voltageAxisY;

    QChart *valueChart;
    QChartView *valueChartView;
    QLineSeries *valueSeries;

    QValueAxis *valueAxisX;
    QValueAxis *valueAxisY;

    QChart *temperatureChart;
    QChartView *temperatureChartView;

    QLineSeries *temperatureSeries;

    // Threshold lines
    QLineSeries *highTempSeries;
    QLineSeries *lowTempSeries;

    QValueAxis *temperatureAxisX;
    QValueAxis *temperatureAxisY;

    void setupTemperatureChart();

    int sampleIndex = 0;

    void setupVoltageChart();
    void setupValueChart();

    void createSensorMarkers();
    void highlightSensor(
        const std::string& sensorName,
        double temperature
        );

protected:
            void resizeEvent(QResizeEvent *event) override;

};

#endif // MAINWINDOW_H
