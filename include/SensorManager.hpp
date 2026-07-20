#pragma once
#include <vector>
#include <memory>
#include <mutex>
#include <string>

#include "Sensor.hpp"

class SensorManager {
private:
	std::vector<std::unique_ptr<Sensor>> sensors;
	mutable std::mutex sensorMutex;

public:
	void addSensor(std::unique_ptr<Sensor> sensor);

	void updateAllSensors();

	std::string getSensorsJson() const;
};