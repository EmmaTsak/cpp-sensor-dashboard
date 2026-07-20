#include "Sensor.hpp"

#include <utility>
#include <algorithm>
#include <numeric>

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

    history.push_back(latestValue);

    if (history.size() > maxHistorySize) {
        history.pop_front();
    }
}

double Sensor::getLatestValue() const {
    return latestValue;
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

const std::deque<double>& Sensor::getHistory() const {
    return history;
}

double Sensor::getMin() const {
    if (history.empty()) {
        return 0.0;
    }

    return *std::min_element(history.begin(), history.end());
}

double Sensor::getMax() const {
    if (history.empty()) {
        return 0.0;
    }

    return *std::max_element(history.begin(), history.end());
}

double Sensor::getAverage() const {
    if (history.empty()) {
        return 0.0;
    }

    double sum = std::accumulate(history.begin(), history.end(), 0.0);

    return sum / history.size();
}