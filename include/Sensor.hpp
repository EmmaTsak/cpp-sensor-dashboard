#pragma once

#include <string>
#include <random>
#include <deque>

struct SensorReading {
    double value;
    std::string timestamp;
};

class Sensor {
private:
    std::string name;
    std::string unit;

    double minValue;
    double maxValue;

    double warningLow;
    double warningHigh;

    double latestValue = 0.0;
    std::string latestTimestamp;

    std::deque<SensorReading> history;
    static constexpr std::size_t maxHistorySize = 10;

public:
    Sensor(
        std::string name,
        std::string unit,
        double minValue,
        double maxValue,
        double warningLow,
        double warningHigh
    );

    std::string getName() const;
    std::string getUnit() const;

    void updateReading();

    double getLatestValue() const;
    std::string getLatestTimestamp() const;

    std::string getStatus() const;

    const std::deque<SensorReading>& getHistory() const;

    double getMin() const;
    double getMax() const;
    double getAverage() const;
};