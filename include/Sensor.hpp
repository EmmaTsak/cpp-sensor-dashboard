#pragma once

#include <string>
#include <random>
#include <deque>
#include <cstddef>

class Sensor {
private:
    std::string name;
    std::string unit;

    double minValue;
    double maxValue;

    double warningLow;
    double warningHigh;

    double latestValue = 0.0;

    std::deque<double> history;
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
    std::string getStatus() const;

    const std::deque<double>& getHistory() const;

    double getMin() const;
    double getMax() const;
    double getAverage() const;
};