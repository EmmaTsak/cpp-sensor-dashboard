#include "Sensor.hpp"

#include <utility>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <chrono>

std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();

    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

    std::tm localTime{};

#ifdef _WIN32
    localtime_s(&localTime, &currentTime);
#else
    localtime_r(&currentTime, &localTime);
#endif

    std::ostringstream timestamp;
    timestamp << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");

    return timestamp.str();
}

Sensor::Sensor(
    std::string name,
    std::string unit,
    double minValue,
    double maxValue,
    double warningLow,
    double warningHigh
)
    : name(std::move(name)),
    unit(std::move(unit)),
    minValue(minValue),
    maxValue(maxValue),
    warningLow(warningLow),
    warningHigh(warningHigh) {
}

std::string Sensor::getName() const {
    return name;
}

std::string Sensor::getUnit() const {
    return unit;
}

void Sensor::updateReading() {
    static std::random_device rd;
    static std::mt19937 generator(rd());

    std::uniform_real_distribution<double> distribution(minValue, maxValue);

    latestValue = distribution(generator);
    latestTimestamp = getCurrentTimestamp();

    SensorReading reading{
        latestValue,
        latestTimestamp
    };

    history.push_back(reading);

    if (history.size() > maxHistorySize) {
        history.pop_front();
    }
}

double Sensor::getLatestValue() const {
    return latestValue;
}

std::string Sensor::getLatestTimestamp() const {
    return latestTimestamp;
}

std::string Sensor::getStatus() const {
    if (latestValue < warningLow) {
        return "LOW";
    }

    if (latestValue > warningHigh) {
        return "HIGH";
    }

    return "OK";
}

const std::deque<SensorReading>& Sensor::getHistory() const {
    return history;
}

double Sensor::getMin() const {
    if (history.empty()) {
        return 0.0;
    }

    auto minReading = std::min_element(
        history.begin(),
        history.end(),
        [](const SensorReading& a, const SensorReading& b) {
            return a.value < b.value;
        }
    );

    return minReading->value;
}

double Sensor::getMax() const {
    if (history.empty()) {
        return 0.0;
    }

    auto maxReading = std::max_element(
        history.begin(),
        history.end(),
        [](const SensorReading& a, const SensorReading& b) {
            return a.value < b.value;
        }
    );

    return maxReading->value;
}

double Sensor::getAverage() const {
    if (history.empty()) {
        return 0.0;
    }

    double sum = std::accumulate(
        history.begin(),
        history.end(),
        0.0,
        [](double total, const SensorReading& reading) {
            return total + reading.value;
        }
    );

    return sum / history.size();
}