#pragma once
#include "../backend/backend.hpp"
#include "../common/common.hpp"
#include "thermodynamics.hpp"
#include <iostream>

class Simulation {
private:
bool simulation_flag = true;
TemperatureController temp_controller;
PressureController pressure_controller;
HumidityController humidity_controller;
unsigned long current_time_millis = 0;

public:
    State state;
    Simulation(Environment env, const double MIN_TEMP, const double MAX_TEMP, const double MIN_PRESSURE, const double MAX_PRESSURE,
                         const double MIN_HUMIDITY, const double MAX_HUMIDITY)
             : temp_controller(MIN_TEMP, MAX_TEMP),
               pressure_controller(MIN_PRESSURE, MAX_PRESSURE),
               humidity_controller(MIN_HUMIDITY, MAX_HUMIDITY),
               state(env, ControlMode::AUTOMATICLY, TemperatureController(MIN_TEMP, MAX_TEMP),
                     PressureController(MIN_PRESSURE, MAX_PRESSURE), HumidityController(MIN_HUMIDITY, MAX_HUMIDITY)) {}

    void simulate(unsigned long milliseconds) {
        const unsigned long MILLIS_IN_SEC = 1000;
        double d_t = (double)milliseconds / MILLIS_IN_SEC;
        current_time_millis += milliseconds;

        if (d_t <= 0.0) {
            return;
        }

        Thermodynamics::update_temperature_with_controller(state, d_t);
        Thermodynamics::update_pressure_with_controller(state, d_t);
        
        std::cout << "Время: " << current_time_millis / MILLIS_IN_SEC << " с,\n"
                  << "Температура: " << state.get_temperature() << " K,\n"
                  << "Целевая температура: " << state.get_needed_temperature() << " K,\n"
                  << "Давление: " << state.get_pressure() << " Pa,\n"
                  << "Целевое давление: " << state.get_needed_pressure() << " Pa,\n"
                  << "Масса: " << state.get_mass() << " kg,\n"
                  << "Теплоемкость: " << state.get_heat_capacity() << " J/(kg·K),\n"
                  << "Тепловыделение: " << state.get_reaction_heat_rate() << " W,\n"
                  << "Нагрев: " << state.get_heating_rate() << " W,\n"
                  << "Охлаждение: " << state.get_cooling_rate() << " W\n\n";
    }

    [[nodiscard]] bool get_simulation_flag() const
    {
        return simulation_flag;
    }
    void set_simulation_flag(bool simulation_flag)
    {
        this->simulation_flag = simulation_flag;
    }
};