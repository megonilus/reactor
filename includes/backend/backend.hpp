#pragma once

class TemperatureController {
private:
    double temp = 0;

public:
    TemperatureController() = default;

    [[nodiscard]] double get_temp() const;
};

class PressureController {
private:
    double pressure = 0;

public:
    PressureController() = default;

    [[nodiscard]] double get_pressure() const;
};

class HumidityController {
private:
    double humidity = 0;

public:
    HumidityController() = default;

    [[nodiscard]] double get_humidity() const;
};