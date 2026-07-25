#include <iostream>
#include <memory>
#include <thread>
#include <atomic>
#include <chrono>
#include <fstream>
#include <exception>

#include "Sensor.hpp"
#include "SensorManager.hpp"
#include "httplib.h"
#include "json.hpp"

using json = nlohmann::json;

bool hasRequiredField(
    const json& object,
    const std::string& fieldName,
    const std::string& expectedType,
    size_t sensorIndex
) {
    if (!object.contains(fieldName)) {
        std::cerr << "Error: Sensor at index "
            << sensorIndex
            << " is missing field '"
            << fieldName
            << "'."
            << std::endl;

        return false;
    }

    if (expectedType == "string" && !object[fieldName].is_string()) {
        std::cerr << "Error: Field '"
            << fieldName
            << "' in sensor at index "
            << sensorIndex
            << " must be a string."
            << std::endl;

        return false;
    }

    if (expectedType == "number" && !object[fieldName].is_number()) {
        std::cerr << "Error: Field '"
            << fieldName
            << "' in sensor at index "
            << sensorIndex
            << " must be a number."
            << std::endl;

        return false;
    }

    return true;
}

bool validateSensorConfig(const json& sensorConfig, size_t sensorIndex) {
    if (!hasRequiredField(sensorConfig, "name", "string", sensorIndex)) {
        return false;
    }

    if (!hasRequiredField(sensorConfig, "unit", "string", sensorIndex)) {
        return false;
    }

    if (!hasRequiredField(sensorConfig, "minValue", "number", sensorIndex)) {
        return false;
    }

    if (!hasRequiredField(sensorConfig, "maxValue", "number", sensorIndex)) {
        return false;
    }

    if (!hasRequiredField(sensorConfig, "warningLow", "number", sensorIndex)) {
        return false;
    }

    if (!hasRequiredField(sensorConfig, "warningHigh", "number", sensorIndex)) {
        return false;
    }

    double minValue = sensorConfig["minValue"].get<double>();
    double maxValue = sensorConfig["maxValue"].get<double>();
    double warningLow = sensorConfig["warningLow"].get<double>();
    double warningHigh = sensorConfig["warningHigh"].get<double>();

    std::string name = sensorConfig["name"].get<std::string>();

    if (minValue >= maxValue) {
        std::cerr << "Error: Sensor '"
            << name
            << "' has minValue greater than or equal to maxValue."
            << std::endl;

        return false;
    }

    if (warningLow >= warningHigh) {
        std::cerr << "Error: Sensor '"
            << name
            << "' has warningLow greater than or equal to warningHigh."
            << std::endl;

        return false;
    }

    if (warningLow < minValue || warningHigh > maxValue) {
        std::cerr << "Warning: Sensor '"
            << name
            << "' has warning range outside generated range."
            << std::endl;
    }

    return true;
}

bool loadSensorsFromConfig(const std::string& configPath, SensorManager& sensorManager) {
    try {
        std::ifstream configFile(configPath);

        if (!configFile.is_open()) {
            std::cerr << "Error: Could not open config file: " << configPath << std::endl;
            return false;
        }

        json config;
        configFile >> config;

        if (!config.contains("sensors") || !config["sensors"].is_array()) {
            std::cerr << "Error: Config file must contain a 'sensors' array." << std::endl;
            return false;
        }

        for (size_t i = 0; i < config["sensors"].size(); ++i) {
            const auto& sensorConfig = config["sensors"][i];

            if (!sensorConfig.is_object()) {
                std::cerr << "Error: Sensor at index "
                    << i
                    << " must be a JSON object."
                    << std::endl;

                return false;
            }

            if (!validateSensorConfig(sensorConfig, i)) {
                return false;
            }

            std::string name = sensorConfig["name"].get<std::string>();
            std::string unit = sensorConfig["unit"].get<std::string>();

            double minValue = sensorConfig["minValue"].get<double>();
            double maxValue = sensorConfig["maxValue"].get<double>();
            double warningLow = sensorConfig["warningLow"].get<double>();
            double warningHigh = sensorConfig["warningHigh"].get<double>();

            sensorManager.addSensor(
                std::make_unique<Sensor>(
                    name,
                    unit,
                    minValue,
                    maxValue,
                    warningLow,
                    warningHigh
                )
            );
        }

        return true;
    }
    catch (const std::exception& error) {
        std::cerr << "Error while loading sensor config: " << error.what() << std::endl;
        return false;
    }
}

int main() {
    SensorManager sensorManager;

    if (!loadSensorsFromConfig("../config/sensors.json", sensorManager)) {
        std::cerr << "Failed to load sensors. Exiting application." << std::endl;
        return 1;
    }

    std::atomic<bool> running = true;

    std::thread sensorThread([&]() {
        while (running) {
            sensorManager.updateAllSensors();

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        });

    httplib::Server server;

    server.Get("/health", [](const httplib::Request&, httplib::Response& response) {
        response.set_content("{\"status\":\"ok\"}", "application/json");
        });

    server.Get("/sensors", [&sensorManager](const httplib::Request&, httplib::Response& response) {
        response.set_header("Access-Control-Allow-Origin", "*");
        response.set_content(sensorManager.getSensorsJson(), "application/json");
        });

    server.Get(R"(/sensors/(\w+)/history)", [&sensorManager](const httplib::Request& request, httplib::Response& response) {
        std::string sensorName = request.matches[1];

        std::string result = sensorManager.getSensorHistoryJsonByName(sensorName);

        if (result.find("Sensor not found") != std::string::npos) {
            response.status = 404;
        }

        response.set_header("Access-Control-Allow-Origin", "*");
        response.set_content(result, "application/json");
        });

    server.Get(R"(/sensors/(\w+))", [&sensorManager](const httplib::Request& request, httplib::Response& response) {
        std::string sensorName = request.matches[1];

        std::string result = sensorManager.getSensorJsonByName(sensorName);

        if (result.find("Sensor not found") != std::string::npos) {
            response.status = 404;
        }

        response.set_header("Access-Control-Allow-Origin", "*");
        response.set_content(result, "application/json");
        });

    std::cout << "Sensor Dashboard backend running at http://localhost:8080\n";
    std::cout << "Open http://localhost:8080/health to test the server.\n";
    std::cout << "Open http://localhost:8080/sensors to see sensor data.\n";

    server.listen("localhost", 8080);

    running = false;

    if (sensorThread.joinable()) {
        sensorThread.join();
    }

    return 0;
}