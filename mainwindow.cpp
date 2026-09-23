#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "sensorData.h"

#include <QTimer>
#include <QPixmap>
#include <QGraphicsScene>
#include <QScrollBar>
#include <QVBoxLayout>

#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QValueAxis>
#include <QPainter>

#include <QPen>
#include <QBrush>
#include <QColor>
#include <QResizeEvent>
#include <QDebug>

#include <fstream>
#include <string>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Make sensor output read-only
    ui->output->setReadOnly(true);

    // Set up aircraft and charts
    setupPlane();
    setupValueChart();
    setupVoltageChart();
    setupTemperatureChart();

    // Pause button should not work until streaming starts
    ui->pauseButton->setEnabled(false);
    ui->pauseButton->setText("Stop");

    // Open CSV file
    sensorFile.open("data/sensor_data_100_rows.csv");

    // Skip CSV header
    if (sensorFile.is_open())
    {
        std::string header;
        std::getline(sensorFile, header);
    }
    else
    {
        ui->output->setText("Could not open sensor data file.");
    }

    // Create timer
    timer = new QTimer(this);

    // Timer event:
    // every timeout reads exactly one sensor row
    connect(timer,
            &QTimer::timeout,
            this,
            &MainWindow::readNextSensor);

    // Start button
    connect(ui->pushButton,
            &QPushButton::clicked,
            this,
            &MainWindow::startSensorData);

    // Stop / Continue button
    connect(ui->pauseButton,
            &QPushButton::clicked,
            this,
            &MainWindow::toggleSensorData);
}


// ---------------------------------------------------------
// AIRCRAFT DISPLAY
// ---------------------------------------------------------

void MainWindow::setupPlane()
{
    planeScene = new QGraphicsScene(this);
    ui->planeView->setScene(planeScene);

    QPixmap plane(":/images/plane.jpg");

    if (plane.isNull())
    {
        qDebug() << "Plane image failed to load";
        return;
    }

    planeItem = planeScene->addPixmap(plane);

    planeScene->setSceneRect(
        planeItem->boundingRect()
        );

    // Disable scrollbars because the image fills the view
    ui->planeView->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
        );

    ui->planeView->setVerticalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
        );

    // Create invisible sensor markers
    createSensorMarkers();

    // Wait until Qt finishes laying out the GUI
    QTimer::singleShot(
        0,
        this,
        [this]()
        {
            fitPlaneToView();
        }
        );
}


void MainWindow::fitPlaneToView()
{
    if (!planeItem)
    {
        return;
    }

    ui->planeView->resetTransform();

    ui->planeView->fitInView(
        planeItem,
        Qt::IgnoreAspectRatio
        );
}


// ---------------------------------------------------------
// START / STOP / CONTINUE
// ---------------------------------------------------------

void MainWindow::startSensorData()
{
    if (!sensorFile.is_open())
    {
        ui->output->append(
            "Could not open sensor data file."
            );

        return;
    }

    // Clear previous text history
    ui->output->clear();

    // Disable Start while stream is active
    ui->pushButton->setEnabled(false);

    // Enable Stop / Continue
    ui->pauseButton->setEnabled(true);
    ui->pauseButton->setText("Stop");

    // Reset VALUE graph
    valueSeries->clear();

    // Reset VOLTAGE graph
    voltageSeries->clear();

    // Reset TEMPERATURE graph
    temperatureSeries->clear();

    // Reset sample counter
    sampleIndex = 0;

    // Reset X axes
    valueAxisX->setRange(0, 20);
    voltageAxisX->setRange(0, 20);
    temperatureAxisX->setRange(0, 20);

    // Reset TEMPERATURE threshold lines
    highTempSeries->clear();
    highTempSeries->append(0, 100);
    highTempSeries->append(20, 100);

    lowTempSeries->clear();
    lowTempSeries->append(0, 0);
    lowTempSeries->append(20, 0);

    // Start one reading per second
    timer->start(1000);
}


void MainWindow::toggleSensorData()
{
    if (timer->isActive())
    {
        // Pause stream
        timer->stop();

        ui->pauseButton->setText(
            "Continue"
            );
    }
    else
    {
        // Continue stream
        timer->start(1000);

        ui->pauseButton->setText(
            "Stop"
            );
    }
}


// ---------------------------------------------------------
// READ ONE SENSOR SAMPLE
// ---------------------------------------------------------

void MainWindow::readNextSensor()
{
    std::string line;

    // Read exactly ONE CSV row per timer event
    if (std::getline(sensorFile, line))
    {
        // Extract fields from the current row
        std::string sensorName =
            getSensorName(line);

        double temperature =
            getTemperature(line);

        double currentValue =
            getValue(line);

        double currentVoltage =
            getVoltage(line);

        // Generate formatted sensor text
        std::string sensorData =
            processSensorLine(line);

        // Append reading to sensor history
        ui->output->append(
            QString::fromStdString(sensorData)
            );

        // Scroll automatically to newest reading
        ui->output->verticalScrollBar()->setValue(
            ui->output->verticalScrollBar()->maximum()
            );

        // Highlight sensor location and temperature status
        highlightSensor(
            sensorName,
            temperature
            );

        // Add points to graphs
        valueSeries->append(
            sampleIndex,
            currentValue
            );

        voltageSeries->append(
            sampleIndex,
            currentVoltage
            );

        temperatureSeries->append(
            sampleIndex,
            temperature
            );


        // Move to next sample
        sampleIndex++;


        // Determine visible X range
        int startX;
        int endX;

        if (sampleIndex > 20)
        {
            startX = sampleIndex - 20;
            endX = sampleIndex;
        }
        else
        {
            startX = 0;
            endX = 20;
        }


        // Update all X axes
        valueAxisX->setRange(
            startX,
            endX
            );

        voltageAxisX->setRange(
            startX,
            endX
            );

        temperatureAxisX->setRange(
            startX,
            endX
            );


        // Update HIGH temperature threshold
        highTempSeries->clear();

        highTempSeries->append(
            startX,
            100
            );

        highTempSeries->append(
            endX,
            100
            );


        // Update LOW temperature threshold
        lowTempSeries->clear();

        lowTempSeries->append(
            startX,
            0
            );

        lowTempSeries->append(
            endX,
            0
            );
    }
    else
    {
        // End of CSV
        timer->stop();

        ui->output->append(
            "No more sensor data."
            );

        // Disable Stop / Continue
        ui->pauseButton->setEnabled(false);
        ui->pauseButton->setText("Stop");

        // Allow another simulation run
        ui->pushButton->setEnabled(true);

        // Hide remaining highlighted sensor
        for (auto& marker : sensorMarkers)
        {
            marker.second->setVisible(false);
        }

        // Clear EOF/failure state
        sensorFile.clear();

        // Move back to beginning
        sensorFile.seekg(0);

        // Skip header again
        std::string header;
        std::getline(sensorFile, header);
    }
}


// ---------------------------------------------------------
// VALUE GRAPH
// ---------------------------------------------------------

void MainWindow::setupValueChart()
{
    // Incoming data line
    valueSeries = new QLineSeries();

    valueSeries->setName(
        "Sensor Value"
        );

    // Chart
    valueChart = new QChart();

    valueChart->addSeries(
        valueSeries
        );

    valueChart->setTitle(
        "Sensor Value Over Time"
        );

    valueChart->legend()->hide();

    // X axis
    valueAxisX = new QValueAxis();

    valueAxisX->setTitleText(
        "Sample"
        );

    valueAxisX->setRange(
        0,
        20
        );

    valueAxisX->setLabelFormat(
        "%d"
        );

    // Y axis
    valueAxisY = new QValueAxis();

    valueAxisY->setTitleText(
        "Value"
        );

    valueAxisY->setRange(
        0,
        120
        );

    // Add axes
    valueChart->addAxis(
        valueAxisX,
        Qt::AlignBottom
        );

    valueChart->addAxis(
        valueAxisY,
        Qt::AlignLeft
        );

    // Attach series to axes
    valueSeries->attachAxis(
        valueAxisX
        );

    valueSeries->attachAxis(
        valueAxisY
        );

    // Display chart
    valueChartView =
        new QChartView(valueChart);

    valueChartView->setRenderHint(
        QPainter::Antialiasing
        );

    // Put chart inside Designer widget
    QVBoxLayout *layout =
        new QVBoxLayout(
            ui->valueChartContainer
            );

    layout->setContentsMargins(
        0,
        0,
        0,
        0
        );

    layout->addWidget(
        valueChartView
        );
}


// ---------------------------------------------------------
// VOLTAGE GRAPH
// ---------------------------------------------------------

void MainWindow::setupVoltageChart()
{
    // Incoming voltage line
    voltageSeries = new QLineSeries();

    voltageSeries->setName(
        "Voltage"
        );

    // Chart
    voltageChart = new QChart();

    voltageChart->addSeries(
        voltageSeries
        );

    voltageChart->setTitle(
        "Sensor Voltage Over Time"
        );

    voltageChart->legend()->hide();

    // X axis
    voltageAxisX = new QValueAxis();

    voltageAxisX->setTitleText(
        "Sample"
        );

    voltageAxisX->setRange(
        0,
        20
        );

    voltageAxisX->setLabelFormat(
        "%d"
        );

    // Y axis
    voltageAxisY = new QValueAxis();

    voltageAxisY->setTitleText(
        "Voltage"
        );

    voltageAxisY->setRange(
        0,
        40
        );

    // Add axes
    voltageChart->addAxis(
        voltageAxisX,
        Qt::AlignBottom
        );

    voltageChart->addAxis(
        voltageAxisY,
        Qt::AlignLeft
        );

    // Attach series
    voltageSeries->attachAxis(
        voltageAxisX
        );

    voltageSeries->attachAxis(
        voltageAxisY
        );

    // Display chart
    voltageChartView =
        new QChartView(voltageChart);

    voltageChartView->setRenderHint(
        QPainter::Antialiasing
        );

    // Put chart inside Designer container
    QVBoxLayout *layout =
        new QVBoxLayout(
            ui->voltageChartContainer
            );

    layout->setContentsMargins(
        0,
        0,
        0,
        0
        );

    layout->addWidget(
        voltageChartView
        );
}

// ---------------------------------------------------------
// TEMPERATURE GRAPH
// ---------------------------------------------------------

void MainWindow::setupTemperatureChart()
{
    // -------------------------
    // Actual temperature series
    // -------------------------
    temperatureSeries = new QLineSeries();

    temperatureSeries->setName(
        "Temperature"
        );


    // -------------------------
    // High temperature threshold
    // -------------------------
    highTempSeries = new QLineSeries();

    highTempSeries->setName(
        "High Temperature"
        );

    QPen highPen(Qt::red);
    highPen.setWidth(2);
    highPen.setStyle(Qt::DashLine);

    highTempSeries->setPen(highPen);


    // -------------------------
    // Low temperature threshold
    // -------------------------
    lowTempSeries = new QLineSeries();

    lowTempSeries->setName(
        "Low Temperature"
        );

    QPen lowPen(Qt::blue);
    lowPen.setWidth(2);
    lowPen.setStyle(Qt::DashLine);

    lowTempSeries->setPen(lowPen);


    // Initial threshold lines
    highTempSeries->append(0, 100);
    highTempSeries->append(20, 100);

    lowTempSeries->append(0, 0);
    lowTempSeries->append(20, 0);


    // -------------------------
    // Create chart
    // -------------------------
    temperatureChart = new QChart();

    temperatureChart->addSeries(
        temperatureSeries
        );

    temperatureChart->addSeries(
        highTempSeries
        );

    temperatureChart->addSeries(
        lowTempSeries
        );

    temperatureChart->setTitle(
        "Sensor Temperature Over Time"
        );


    // -------------------------
    // X axis
    // -------------------------
    temperatureAxisX = new QValueAxis();

    temperatureAxisX->setTitleText(
        "Sample"
        );

    temperatureAxisX->setRange(
        0,
        20
        );

    temperatureAxisX->setLabelFormat(
        "%d"
        );


    // -------------------------
    // Y axis
    // -------------------------
    temperatureAxisY = new QValueAxis();

    temperatureAxisY->setTitleText(
        "Temperature"
        );

    temperatureAxisY->setRange(
        -20,
        120
        );


    // -------------------------
    // Add axes
    // -------------------------
    temperatureChart->addAxis(
        temperatureAxisX,
        Qt::AlignBottom
        );

    temperatureChart->addAxis(
        temperatureAxisY,
        Qt::AlignLeft
        );


    // Actual temperature uses same axes
    temperatureSeries->attachAxis(
        temperatureAxisX
        );

    temperatureSeries->attachAxis(
        temperatureAxisY
        );


    // High threshold uses same axes
    highTempSeries->attachAxis(
        temperatureAxisX
        );

    highTempSeries->attachAxis(
        temperatureAxisY
        );


    // Low threshold uses same axes
    lowTempSeries->attachAxis(
        temperatureAxisX
        );

    lowTempSeries->attachAxis(
        temperatureAxisY
        );


    // -------------------------
    // Create visible chart
    // -------------------------
    temperatureChartView =
        new QChartView(
            temperatureChart
            );

    temperatureChartView->setRenderHint(
        QPainter::Antialiasing
        );


    // -------------------------
    // Add chart to Designer widget
    // -------------------------
    QVBoxLayout *layout =
        new QVBoxLayout(
            ui->temperatureChartContainer
            );

    layout->setContentsMargins(
        0,
        0,
        0,
        0
        );

    layout->addWidget(
        temperatureChartView
        );
}

// ---------------------------------------------------------
// SENSOR MARKERS
// ---------------------------------------------------------

void MainWindow::createSensorMarkers()
{
    const int radius = 55;

    QPen pen(Qt::green);
    pen.setWidth(8);

    QBrush brush(Qt::green);

    sensorMarkers["NOSE_RADAR"] =
        planeScene->addEllipse(
            822 - radius,
            189 - radius,
            radius * 2,
            radius * 2,
            pen,
            brush
            );

    sensorMarkers["ENGINE_LEFT"] =
        planeScene->addEllipse(
            600 - radius,
            543 - radius,
            radius * 2,
            radius * 2,
            pen,
            brush
            );

    sensorMarkers["ENGINE_RIGHT"] =
        planeScene->addEllipse(
            1056 - radius,
            543 - radius,
            radius * 2,
            radius * 2,
            pen,
            brush
            );

    sensorMarkers["LEFT_WING"] =
        planeScene->addEllipse(
            86 - radius,
            966 - radius,
            radius * 2,
            radius * 2,
            pen,
            brush
            );

    sensorMarkers["RIGHT_WING"] =
        planeScene->addEllipse(
            1557 - radius,
            966 - radius,
            radius * 2,
            radius * 2,
            pen,
            brush
            );

    sensorMarkers["TAIL_SENSOR"] =
        planeScene->addEllipse(
            822 - radius,
            1441 - radius,
            radius * 2,
            radius * 2,
            pen,
            brush
            );

    // Put markers above plane image
    // and hide them initially
    for (auto& marker : sensorMarkers)
    {
        marker.second->setZValue(1);
        marker.second->setVisible(false);
    }
}


void MainWindow::highlightSensor(
    const std::string& sensorName,
    double temperature)
{
    // Hide all markers
    for (auto& marker : sensorMarkers)
    {
        marker.second->setVisible(false);
    }

    // Find current sensor marker
    auto currentSensor =
        sensorMarkers.find(sensorName);

    if (currentSensor != sensorMarkers.end())
    {
        QColor markerColor;

        // Determine color from temperature
        if (temperature > 100)
        {
            markerColor = Qt::red;
        }
        else if (temperature < 0)
        {
            markerColor = Qt::blue;
        }
        else
        {
            markerColor = Qt::green;
        }

        QPen pen(markerColor);
        pen.setWidth(8);

        QBrush brush(markerColor);

        currentSensor->second->setPen(
            pen
            );

        currentSensor->second->setBrush(
            brush
            );

        currentSensor->second->setVisible(
            true
            );
    }
}


// ---------------------------------------------------------
// WINDOW RESIZING
// ---------------------------------------------------------

void MainWindow::resizeEvent(
    QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    fitPlaneToView();
}


// ---------------------------------------------------------
// DESTRUCTOR
// ---------------------------------------------------------

MainWindow::~MainWindow()
{
    if (sensorFile.is_open())
    {
        sensorFile.close();
    }

    delete ui;
}