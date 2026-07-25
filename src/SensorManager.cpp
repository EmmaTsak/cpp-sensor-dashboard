#include "SensorManager.hpp"
#include "json.hpp"

#include <cmath>

using json = nlohmann::json;

double roundToTwoDecimals(double value) {
    return std::round(value * 100.0) / 100.0;
}

json sensorToJson(const Sensor& sensor) {
    json historyJson = json::array();

    const auto& history = sensor.getHistory();

    for (const auto& reading : history) {
        json readingJson;

        readingJson["value"] = roundToTwoDecimals(reading.value);
        readingJson["timestamp"] = reading.timestamp;

        historyJson.push_back(readingJson);
    }

    json sensorJson;
    sensorJson["name"] = sensor.getName();
    sensorJson["value"] = roundToTwoDecimals(sensor.getLatestValue());
    sensorJson["unit"] = sensor.getUnit();
    sensorJson["status"] = sensor.getStatus();
    sensorJson["timestamp"] = sensor.getLatestTimestamp();
    sensorJson["history"] = historyJson;
    sensorJson["min"] = roundToTwoDecimals(sensor.getMin());
    sensorJson["max"] = roundToTwoDecimals(sensor.getMax());
    sensorJson["average"] = roundToTwoDecimals(sensor.getAverage());

    return sensorJson;
}

void SensorManager::addSensor(std::unique_ptr<Sensor> sensor) {
    std::lock_guard<std::mutex> lock(sensorMutex);

    sensors.push_back(std::move(sensor));
}

void SensorManager::updateAllSensors() {
    std::lock_guard<std::mutex> lock(sensorMutex);

    for (auto& sensor : sensors) {
        sensor->updateReading();
    }
}

std::string SensorManager::getSensorsJson() const {
    std::lock_guard<std::mutex> lock(sensorMutex);

    json response;
    response["sensors"] = json::array();

    for (const auto& sensor : sensors) {
        response["sensors"].push_back(sensorToJson(*sensor));
    }

    return response.dump(2);
}

std::string SensorManager::getSensorJsonByName(const std::string& sensorName) const {
    std::lock_guard<std::mutex> lock(sensorMutex);

    for (const auto& sensor : sensors) {
        if (sensor->getName() == sensorName) {
            json response = sensorToJson(*sensor);
            return response.dump(2);
        }
    }

    json errorResponse;
    errorResponse["error"] = "Sensor not found";

    return errorResponse.dump(2);
}

std::string SensorManager::getSensorHistoryJsonByName(const std::string& sensorName) const {
    std::lock_guard<std::mutex> lock(sensorMutex);

    for (const auto& sensor : sensors) {
        if (sensor->getName() == sensorName) {
            json response;
            response["name"] = sensor->getName();
            response["unit"] = sensor->getUnit();
            response["history"] = json::array();

            const auto& history = sensor->getHistory();

            for (const auto& reading : history) {
                json readingJson;
                readingJson["value"] = roundToTwoDecimals(reading.value);
                readingJson["timestamp"] = reading.timestamp;

                response["history"].push_back(readingJson);
            }

            return response.dump(2);
        }
    }

    json errorResponse;
    errorResponse["error"] = "Sensor not found";

    return errorResponse.dump(2);
}