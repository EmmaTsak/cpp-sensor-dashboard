#include "SensorManager.hpp"
#include "json.hpp"

#include <cmath>

using json = nlohmann::json;

double roundToTwoDecimals(double value) {
    return std::round(value * 100.0) / 100.0;
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
        json historyJson = json::array();

        const auto& history = sensor->getHistory();

        for (double reading : history) {
            historyJson.push_back(roundToTwoDecimals(reading));
        }

        json sensorJson;
        sensorJson["name"] = sensor->getName();
        sensorJson["value"] = roundToTwoDecimals(sensor->getLatestValue());
        sensorJson["unit"] = sensor->getUnit();
        sensorJson["status"] = sensor->getStatus();
        sensorJson["history"] = historyJson;
        sensorJson["min"] = roundToTwoDecimals(sensor->getMin());
        sensorJson["max"] = roundToTwoDecimals(sensor->getMax());
        sensorJson["average"] = roundToTwoDecimals(sensor->getAverage());

        response["sensors"].push_back(sensorJson);
    }

    return response.dump(2);
}