#pragma once
#include "../backend/backend.hpp"
#include "../common/common.hpp"
#include "thermodynamics.hpp"
#include <iostream>

class Simulation {
private:
    State state;
    bool simulation_flag = true;
    TemperatureController temp_controller;
    PressureController pressure_controller;
    HumidityController humidity_controller;
    
public:
    Simulation(Environment env, const double MIN_TEMP, const double MAX_TEMP, const double MIN_PRESSURE, const double MAX_PRESSURE,
                         const double MIN_HUMIDITY, const double MAX_HUMIDITY)
             : state(env, ControlMode::AUTOMATICLY, TemperatureController(MIN_TEMP, MAX_TEMP),
                     PressureController(MIN_PRESSURE, MAX_PRESSURE), HumidityController(MIN_HUMIDITY, MAX_HUMIDITY)),
               temp_controller(MIN_TEMP, MAX_TEMP),
               pressure_controller(MIN_PRESSURE, MAX_PRESSURE),
               humidity_controller(MIN_HUMIDITY, MAX_HUMIDITY) {
        
        const double PROPORTIONAL_PARAM = 2.0;
        const double INTEGRAL_PARAM = 0.5;
        const double DERIVATIVE_PARAM = 2.1;
        
        PidParameters pid_params{};
        pid_params.set_proportional(PROPORTIONAL_PARAM)
                  .set_integral(INTEGRAL_PARAM)
                  .set_derivative(DERIVATIVE_PARAM);
        temp_controller.set_pid_parameters(pid_params);
    }
    void simulate(long milliseconds) {
        const double MILLIS_IN_SEC = 1000.0;
        double d_t = (double)milliseconds / MILLIS_IN_SEC;

        temp_controller.update_startup_time(d_t);

        Thermodynamics::update_temperature_with_controller(state, temp_controller, d_t);
        
        std::cout << "Время: " << milliseconds << " мс,\n"
                  << "Температура: " << state.get_temperature() << " K,\n"
                  << "Целевая температура: " << state.get_needed_temperature() << " K,\n"
                  << "Масса: " << state.get_mass() << " kg,\n"
                  << "Теплоемкость: " << state.get_heat_capacity() << " J/(kg·K),\n"
                  << "Тепловыделение: " << state.get_reaction_heat_rate() << " W,\n"
                  << "Нагрев: " << state.get_heating_rate() << " W,\n"
                  << "Охлаждение: " << state.get_cooling_rate() << " W\n\n";
    }

    [[nodiscard]] bool get_simulation_flag() const { return simulation_flag; }
    void set_simulation_flag(bool simulation_flag) { this->simulation_flag = simulation_flag; }
};