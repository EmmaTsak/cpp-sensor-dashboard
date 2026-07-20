#include <iostream>
#include <memory>
#include <thread>
#include <atomic>
#include <chrono>

#include "Sensor.hpp"
#include "SensorManager.hpp"
#include "httplib.h"

int main() {
    SensorManager sensorManager;

    sensorManager.addSensor(std::make_unique<Sensor>("Temperature", "C", 10.0, 40.0, 20.0, 30.0));
    sensorManager.addSensor(std::make_unique<Sensor>("Humidity", "%", 10.0, 90.0, 30.0, 65.0));
    sensorManager.addSensor(std::make_unique<Sensor>("Voltage", "V", 2.5, 3.8, 3.1, 3.5));

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