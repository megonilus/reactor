#pragma once
#include "../backend/backend.hpp"
#include <cstdint>
#include <utility>

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
private:
    struct Environment {
        double mass;                            // kg
        double temperature;                     // K
        double needed_temperature;              // K
        double pressure;                        // Pa
        double needed_pressure;                 // Pa
        double humidity;                        // %
        double needed_humidity;                 // %
        double energy_consumption;              // W
        double max_energy_consumption;          // W
    } environment;
    
    ControlMode control_mode;
    StatusMode status_mode = StatusMode::NORMAL;

    TemperatureController temp_controller;
    PressureController pressure_controller;
    HumidityController humidity_controller;
    
    public:
    State(Environment environment, ControlMode control_mode,
        TemperatureController temp_controller, PressureController pressure_controller, HumidityController humidity_controller)
        : environment(environment), control_mode(control_mode), temp_controller(std::move(temp_controller)),
        pressure_controller(std::move(pressure_controller)), humidity_controller(std::move(humidity_controller)) {}
        
    [[nodiscard]] double get_mass() const { return environment.mass; }
    void set_mass(double mass) { environment.mass =
                                 control_mode == ControlMode::MANUAL ?
                                 mass : environment.mass; }
                                 
    [[nodiscard]] double get_temperature() const { return environment.temperature; }
    void set_temperature(double temperature) { environment.temperature =
                                               control_mode == ControlMode::MANUAL ?
                                               temperature : environment.temperature; }

    [[nodiscard]] double get_needed_temperature() const { return environment.needed_temperature; }

    [[nodiscard]] double get_pressure() const { return environment.pressure; }
    void set_pressure(double pressure) { environment.pressure =
                                         control_mode == ControlMode::MANUAL ?
                                         pressure : environment.pressure; }

    [[nodiscard]] double get_needed_pressure() const { return environment.needed_pressure; }

    [[nodiscard]] double get_humidity() const { return environment.humidity; }
    void set_humidity(double humidity) { environment.humidity =
                                         control_mode == ControlMode::MANUAL ?
                                         humidity : environment.humidity; }

    [[nodiscard]] double get_needed_humidity() const { return environment.needed_humidity; }

    [[nodiscard]] double get_energy_consumption() const { return environment.energy_consumption; }
    void set_energy_consumption(double energy_consumption) { environment.energy_consumption =
                                                             control_mode == ControlMode::MANUAL ?
                                                             energy_consumption : environment.energy_consumption; }

    [[nodiscard]] double get_max_energy_consumption() const { return environment.max_energy_consumption; }

    [[nodiscard]] ControlMode get_control_mode() const { return control_mode; }
    void set_control_mode(ControlMode control_mode) { this->control_mode =
                                                      control_mode == ControlMode::MANUAL ?
                                                      control_mode : this->control_mode; }

    [[nodiscard]] StatusMode get_status_mode() const { return status_mode; }
    void set_status_mode(StatusMode status_mode) { this->status_mode =
                                                   control_mode == ControlMode::MANUAL ?
                                                   status_mode : this->status_mode; }
};
