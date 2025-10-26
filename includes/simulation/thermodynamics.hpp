#pragma once
#include "../common/common.hpp"
#include <cmath>

class Thermodynamics {
private:
    static constexpr double STEFAN_BOLTZMANN = 5.670374419e-8;
    static constexpr double GAS_CONSTANT = 8.314462618;
    static constexpr double DEFAULT_EMISSIVITY = 0.1;
    static constexpr double WATER_FRACTION_DEFAULT = 0.7;
    static constexpr double ORGANIC_FRACTION_DEFAULT = 0.3;
    static constexpr double REACTION_RATE_CONSTANT_DEFAULT = 1e-3;
    static constexpr double ACTIVATION_ENERGY_DEFAULT = 50000.0;
    static constexpr double HEAT_OF_REACTION_DEFAULT = 100000.0;
    static constexpr double VISCOSITY_DEFAULT = 1e-3;
    static constexpr double CHARACTERISTIC_LENGTH = 0.1;
    static constexpr double DITTUS_BOELTER_COEFFICIENT = 0.023;
    static constexpr double REYNOLDS_EXPONENT = 0.8;
    static constexpr double PRANDTL_EXPONENT = 0.4;
    
public:
    static double calculate_conduction_heat_loss(const State& state) {
        double thermal_conductivity = state.get_wall_thermal_conductivity();
        double surface_area = state.get_surface_area();
        double wall_thickness = state.get_wall_thickness();
        double temperature_internal = state.get_temperature();
        double temperature_ambient = state.get_ambient_temperature();
        
        if (wall_thickness <= 0.0) {
            return 0.0;
        }
        
        return thermal_conductivity * surface_area * (temperature_internal - temperature_ambient) / wall_thickness;
    }
    
    static double calculate_convection_heat_loss(const State& state) {
        double heat_transfer_coefficient = state.get_heat_transfer_coefficient();
        double surface_area = state.get_surface_area();
        double temperature_surface = state.get_temperature();
        double temperature_ambient = state.get_ambient_temperature();
        
        return heat_transfer_coefficient * surface_area * (temperature_surface - temperature_ambient);
    }
    
    static double calculate_radiation_heat_loss(const State& state, double emissivity = DEFAULT_EMISSIVITY) {
        double surface_area = state.get_surface_area();
        double temperature = state.get_temperature();
        double temperature_ambient = state.get_ambient_temperature();
        
        return STEFAN_BOLTZMANN * emissivity * surface_area * (std::pow(temperature, 4) - std::pow(temperature_ambient, 4));
    }
    
    static double calculate_total_heat_loss(const State& state) {
        return calculate_conduction_heat_loss(state) + 
               calculate_convection_heat_loss(state) + 
               calculate_radiation_heat_loss(state);
    }
    
    static double calculate_temperature_change(const State& state, double delta_time) {
        double mass = state.get_mass();
        double heat_capacity = state.get_heat_capacity();
        
        if (mass <= 0.0 || heat_capacity <= 0.0) {
            return 0.0;
        }
        
        double heat_input = state.get_heating_rate() + state.get_reaction_heat_rate();
        
        double heat_loss = calculate_total_heat_loss(state) + state.get_cooling_rate();
        
        double net_heat_flow = heat_input - heat_loss;
        
        return (net_heat_flow * delta_time) / (mass * heat_capacity);
    }
    
    static double calculate_mixture_heat_capacity(double water_fraction = WATER_FRACTION_DEFAULT, 
                                                  double organic_fraction = ORGANIC_FRACTION_DEFAULT) {
        const double WATER_CP = 4180.0;
        const double ORGANIC_CP = 2000.0;
        
        return (water_fraction * WATER_CP) + (organic_fraction * ORGANIC_CP);
    }
    
    static double calculate_reaction_heat_rate(const State& state, 
                                               double reaction_rate_constant = REACTION_RATE_CONSTANT_DEFAULT,
                                               double activation_energy = ACTIVATION_ENERGY_DEFAULT) {
        double temperature = state.get_temperature();
        double gas_constant = GAS_CONSTANT;
        
        double rate_constant = reaction_rate_constant * std::exp(-activation_energy / (gas_constant * temperature));
        
        double heat_of_reaction = HEAT_OF_REACTION_DEFAULT;
        return rate_constant * state.get_mass() * heat_of_reaction;
    }
    
    static double calculate_heat_transfer_coefficient(const State& state, 
                                                      double flow_velocity = 1.0,
                                                      double viscosity = VISCOSITY_DEFAULT) {
        double thermal_conductivity = state.get_thermal_conductivity();
        double density = state.get_mass() / state.get_volume();
        
        double reynolds_number = density * flow_velocity * CHARACTERISTIC_LENGTH / viscosity;
        
        double prandtl_number = viscosity * state.get_heat_capacity() / thermal_conductivity;
        
        double nusselt_number = DITTUS_BOELTER_COEFFICIENT * std::pow(reynolds_number, REYNOLDS_EXPONENT) * std::pow(prandtl_number, PRANDTL_EXPONENT);
        
        return nusselt_number * thermal_conductivity / CHARACTERISTIC_LENGTH;
    }
    
    static void update_temperature(State& state, double delta_time) {
        state.set_heat_capacity(calculate_mixture_heat_capacity());
        state.set_reaction_heat_rate(calculate_reaction_heat_rate(state));
        state.set_heat_transfer_coefficient(calculate_heat_transfer_coefficient(state));
        
        double temperature_change = calculate_temperature_change(state, delta_time);
        
        double new_temperature = state.get_temperature() + temperature_change;
        state.set_temperature(new_temperature);
    }
    
    template<typename T>
    static void update_temperature_with_controller(State& state, T& controller, double delta_time) {
        state.set_heat_capacity(calculate_mixture_heat_capacity());
        state.set_reaction_heat_rate(calculate_reaction_heat_rate(state));
        state.set_heat_transfer_coefficient(calculate_heat_transfer_coefficient(state));
        
        auto [heating_power, cooling_power] = controller.calculate_parallel_control_output(state, delta_time);
        
        state.set_heating_rate(heating_power);
        state.set_cooling_rate(cooling_power);
        
        double temperature_change = calculate_temperature_change(state, delta_time);
        
        double new_temperature = state.get_temperature() + temperature_change;
        state.set_temperature(new_temperature);
    }
};