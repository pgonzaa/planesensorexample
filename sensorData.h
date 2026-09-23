#ifndef SENSORDATA_H
#define SENSORDATA_H

#include <string>

std::string processSensorLine(const std::string& line);
std::string getSensorName(const std::string& line);
double getVoltage(const std::string& line);
double getValue(const std::string& line);
double getTemperature(const std::string& line);

#endif