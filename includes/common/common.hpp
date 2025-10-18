#pragma once
#include "../backend/backend.hpp"
#include <cstdint>

enum class ControlMode : std::uint8_t {
    AUTOMATICLY,
    MANUAL
};

enum class StatusMode : std::uint8_t {
    NORMAL,
    WARNING,
    CRITICAL
};

struct State {
    ControlMode control_mode;
    StatusMode status_mode;

    TemperatureController temp_controller();
    PressureController pressure_controller();
    HumidityController humidity_controller();
};