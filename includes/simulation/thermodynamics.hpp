#pragma once
#include "../common/common.hpp"
#include <algorithm>
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
    static constexpr double MOLAR_MASS_WATER = 0.018015;
    static constexpr double LATENT_HEAT_WATER = 2260000.0;
    
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

        if (temperature <= 0.0) {
            return 0.0;
        }
        
        double rate_constant = reaction_rate_constant * std::exp(-activation_energy / (GAS_CONSTANT * temperature));
        
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

    static double calculate_saturation_pressure(double temperature_kelvin) {
        // Константы для воды (диапазон 1C - 374C)
        // log10(P_mmHg) = A - B / (C + T_celsius)
        const double A = 8.07131;
        const double B = 1730.63;
        const double C = 233.426;

        double temp_c = temperature_kelvin - 273.15;
        temp_c = std::max(temp_c, 1.0); // защита границ

        double p_mmHg = std::pow(10, A - (B / (C + temp_c)));
        double p_pascal = p_mmHg * 133.322; // конвертация в Паскали
        return p_pascal;
    }
    
    static void update_temperature(State& state, double delta_time) {
        state.set_heat_capacity(calculate_mixture_heat_capacity());
        state.set_reaction_heat_rate(calculate_reaction_heat_rate(state));
        state.set_heat_transfer_coefficient(calculate_heat_transfer_coefficient(state));
        
        double temperature_change = calculate_temperature_change(state, delta_time);
        
        double new_temperature = state.get_temperature() + temperature_change;
        state.set_temperature(new_temperature);
    }
    
    static void update_temperature_with_controller(State& state, double delta_time) {
        state.set_heat_capacity(calculate_mixture_heat_capacity());
        state.set_reaction_heat_rate(calculate_reaction_heat_rate(state));
        state.set_heat_transfer_coefficient(calculate_heat_transfer_coefficient(state));
        
        auto [heating_power, cooling_power] = TemperatureController::calculate_parallel_control_output<State>(state);
        
        state.set_heating_rate(heating_power);
        state.set_cooling_rate(cooling_power);
        
        double temperature_change = calculate_temperature_change(state, delta_time);
        
        double new_temperature = state.get_temperature() + temperature_change;
        state.set_temperature(new_temperature);
    }

    static double calculate_pressure(const State& state) {
        double gas_const = state.get_specific_gas_constant();
        double volume = state.get_volume();
        double temp = state.get_temperature();
        double mass = state.get_mass();

        if (gas_const <= 0.0 || volume <= 0.0 || temp <= 0.0) {
            return state.get_pressure(); // не меняем
        }

        return (mass * gas_const * temp) / volume;
    }

    // Обновляет массу/давление, руководствуясь регулятором давления.
    static void update_pressure_with_controller(State& state, double delta_time) {
        // Рассчитать изменение массы, которое предложит контроллер
        double mass_delta = PressureController::calculate_mass_flow_output<State>(state, delta_time);

        // Применяем изменение массы
        double new_mass = state.get_mass() + mass_delta;
        new_mass = std::max(new_mass, 1e-6); // защита от нулевой или отрицательной массы
        state.set_mass(new_mass);

        // Пересчитать давление и сохранить
        double new_pressure = calculate_pressure(state);
        state.set_pressure(new_pressure);
    }

    static void update_humidity_with_controller(State& state, double delta_time) {
        double temp = state.get_temperature();
        double vol = state.get_volume();
        
        // Рассчитываем давление насыщенного пара (P_sat) при текущей T
        double p_sat = calculate_saturation_pressure(temp);
        
        // Защита от физически некорректных значений при очень низких температурах
        p_sat = std::max(p_sat, 0.1); 

        // Рассчитываем МАКСИМАЛЬНУЮ массу воды (газообразной), которую может вместить реактор
        // m_max = (P_sat * V * M) / (R * T)
        double max_water_vapor_mass = (p_sat * vol * MOLAR_MASS_WATER) / (GAS_CONSTANT * temp);

        // Получаем текущую массу воды на основе текущей влажности
        // Humidity = (m_current / m_max) * 100 => m_current = (Humidity / 100) * m_max
        double current_water_mass = (state.get_humidity() / 100.0) * max_water_vapor_mass;

        // Спрашиваем контроллер, сколько воды добавить/убрать
        // Но теперь передаем ему max_mass, чтобы он понимал масштаб
        double water_flow_rate = HumidityController::calculate_water_injection_rate<State>(state, delta_time, max_water_vapor_mass);
        
        double mass_change = water_flow_rate * delta_time;

        // Применяем изменение массы
        double new_water_mass = current_water_mass + mass_change;

        // Ограничиваем физикой: масса не может быть меньше 0
        new_water_mass = std::max(new_water_mass, 0.0);
        
        // Если масса превышает максимум (100% влажности), излишек конденсируется (влажность остается 100%)
        // В более сложной модели излишек стал бы жидкостью, но здесь просто ограничиваем пар.
        new_water_mass = std::min(new_water_mass, max_water_vapor_mass);

        // Обновляем общую массу реактора
        // delta_real = то, что реально изменилось
        double real_mass_delta = new_water_mass - current_water_mass;
        state.set_mass(state.get_mass() + real_mass_delta);

        // Пересчитываем влажность
        double new_humidity = (new_water_mass / max_water_vapor_mass) * 100.0;
        state.set_humidity(new_humidity);

        // Термодинамический эффект (испарение охлаждает, конденсация нагревает)
        // Q = dm * L. Если delta > 0 (испарение) -> теряем тепло.
        // Эффект должен быть ощутимым, но не ломать симуляцию.
        double energy_change = -real_mass_delta * LATENT_HEAT_WATER;
        
        // dT = Q / (m * c)
        double temp_correction = energy_change / (state.get_mass() * state.get_heat_capacity());
        
        // Сглаживание температурного скачка (чтобы не было взрыва значений)
        state.set_temperature(state.get_temperature() + temp_correction);
    }
};