#include "sensorData.h"

#include <sstream>
#include <string>

std::string processSensorLine(const std::string& line)
{
    std::ostringstream output;

    std::stringstream ss(line);

    std::string sensorName;
    std::string value;
    std::string temp;
    std::string voltage;
    std::string hasWeapons;

    std::getline(ss, sensorName, ',');
    std::getline(ss, value, ',');
    std::getline(ss, temp, ',');
    std::getline(ss, voltage, ',');
    std::getline(ss, hasWeapons, ',');

    double tempNumber = std::stod(temp);
    double valueNumber = std::stod(value);
    double voltageNumber = std::stod(voltage);

    bool weapons = (hasWeapons == "true");

    output << "Sensor: " << sensorName << "\n";
    output << "Value: " << valueNumber << "\n";
    output << "Temp: " << tempNumber << "\n";

    if (tempNumber < 0)
    {
        output << "WARNING: Low Temperature\n";
    }
    else if (tempNumber > 100)
    {
        output << "WARNING: High Temperature\n";
    }

    output << "Voltage: " << voltageNumber << "\n";
    output << "Has Weapons: "
           << (weapons ? "Yes" : "No")
           << "\n";

    output << "------------------------\n";

    return output.str();
}

std::string getSensorName(const std::string& line)
{
    std::stringstream ss(line);

    std::string sensorName;

    std::getline(ss, sensorName, ',');

    return sensorName;
}

double getVoltage(const std::string& line)
{
    std::stringstream ss(line);

    std::string sensorName;
    std::string value;
    std::string temp;
    std::string voltage;

    std::getline(ss, sensorName, ',');
    std::getline(ss, value, ',');
    std::getline(ss, temp, ',');
    std::getline(ss, voltage, ',');

    return std::stod(voltage);
}

double getValue(const std::string& line)
{
    std::stringstream ss(line);

    std::string sensorName;
    std::string value;

    std::getline(ss, sensorName, ',');
    std::getline(ss, value, ',');

    return std::stod(value);
}

double getTemperature(const std::string& line)
{
    std::stringstream ss(line);

    std::string sensorName;
    std::string value;
    std::string temp;

    std::getline(ss, sensorName, ',');
    std::getline(ss, value, ',');
    std::getline(ss, temp, ',');

    return std::stod(temp);
}