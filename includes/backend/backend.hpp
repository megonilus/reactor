#pragma once
#include <algorithm>
#include <utility>
#include <cmath>

struct State;

class Sensor {
private:
    struct Range {
        double min_value;
        double max_value;
    } range;
    
public:
    explicit Sensor(Range range)
                  : range(range) {}

    [[nodiscard]] double get_min_value() const { return range.min_value; }
    [[nodiscard]] double get_max_value() const { return range.max_value; }
};

class Controller {
private:
    Sensor sensor;
    bool control_state;

public:
    Controller(Sensor sensor, bool control_state)
             : sensor(sensor), control_state(control_state) {}
    Controller(const Controller& other) = default;
    Controller& operator =(const Controller& other) = default;
    Controller(Controller&& other) noexcept = default;
    Controller& operator =(Controller&& other) = default;
    virtual ~Controller() = default;

    [[nodiscard]] Sensor get_sensor() { return sensor; }
    [[nodiscard]] bool have_control() const { return control_state; }
    void set_control(bool state) { control_state = state; }
};

class TemperatureController : public Controller {    
public:
    TemperatureController(double min, double max, bool control_state = true)
                        : Controller(Sensor({.min_value=min, .max_value=max}), control_state) {}

    template<typename T>
    [[nodiscard]] double get_value(T& state) { return std::max(std::min(state.get_temperature(), get_max_value()), get_min_value()); }
    [[nodiscard]] double get_min_value() { return get_sensor().get_min_value(); }
    [[nodiscard]] double get_max_value() { return get_sensor().get_max_value(); }

    template<typename T = State>
    static std::pair<double, double> calculate_parallel_control_output(T& state) {
        constexpr double STEFAN_BOLTZMANN = 5.670374419e-8;
        constexpr double DEFAULT_EMISSIVITY = 0.1;
        constexpr double GAS_CONSTANT = 8.314462618;
        constexpr double REACTION_RATE_CONSTANT_DEFAULT = 1e-3;
        constexpr double ACTIVATION_ENERGY_DEFAULT = 50000.0;
        constexpr double HEAT_OF_REACTION_DEFAULT = 100000.0;

        double heating_power = 0.0;
        double cooling_power = 0.0;
        double max_power = state.get_max_energy_consumption();
        double needed = state.get_needed_temperature();
        double current = state.get_temperature();
        double diff = needed - current;
        double ambient = state.get_ambient_temperature();
        double surface_area = state.get_surface_area();
        double wall_thickness = state.get_wall_thickness();
        double wall_thermal_conductivity = state.get_wall_thermal_conductivity();
        double heat_transfer_coefficient = state.get_heat_transfer_coefficient();
        double mass = state.get_mass();

        double conduction = (wall_thickness > 0.0) ? wall_thermal_conductivity * surface_area * (needed - ambient) / wall_thickness : 0.0;
        double convection = heat_transfer_coefficient * surface_area * (needed - ambient);
        double radiation = STEFAN_BOLTZMANN * DEFAULT_EMISSIVITY * surface_area * (std::pow(needed, 4) - std::pow(ambient, 4));
        double loss_needed = conduction + convection + radiation;

        double exp_term = (needed > 0.0) ? std::exp(-ACTIVATION_ENERGY_DEFAULT / (GAS_CONSTANT * needed)) : 0.0;
        double rate = REACTION_RATE_CONSTANT_DEFAULT * exp_term;
        double reac_needed = rate * mass * HEAT_OF_REACTION_DEFAULT;

        double required_heating = std::max(0.0, loss_needed - reac_needed);
        double required_cooling = std::max(0.0, reac_needed - loss_needed);
        double kp = max_power / 50.0;

        if (diff >= 0) {
            heating_power = required_heating + kp * diff;
            heating_power = std::max(0.0, std::min(heating_power, max_power));
            cooling_power = 0.0;
        } else {
            double diff_cool = -diff;
            cooling_power = required_cooling + kp * diff_cool;
            cooling_power = std::max(0.0, std::min(cooling_power, max_power));
            heating_power = 0.0;
        }

        return {heating_power, cooling_power};
    }
};

class PressureController : public Controller {
public:
    PressureController(double min, double max, bool control_state = true)
                     : Controller(Sensor({.min_value=min, .max_value=max}), control_state) {}

    template<typename T>
    [[nodiscard]] double get_value(T& state) { return std::max(std::min(state.get_pressure(), get_max_value()), get_min_value()); }
    [[nodiscard]] double get_min_value() { return get_sensor().get_min_value(); }
    [[nodiscard]] double get_max_value() { return get_sensor().get_max_value(); }

    // Ограничивает массовый поток по разумной фракции массы в секунду.
    template<typename T = State>
    static double calculate_mass_flow_output(T& state, double delta_time) {
        double gas_const = state.get_specific_gas_constant();
        double volume = state.get_volume();
        double temp = state.get_temperature();
        double current_pressure = state.get_pressure();
        double needed_pressure = state.get_needed_pressure();

        if (gas_const <= 0.0 || volume <= 0.0 || temp <= 0.0 || delta_time <= 0.0) {
            return 0.0;
        }

        double pressure_error = needed_pressure - current_pressure;

        // Коэффициент, определяющий "скорость реакции" системы на ошибку давления
        constexpr double Kp = 0.002; // можно варьировать 0.001..0.01 для плавности

        // Массовый поток (кг/с), пропорциональный ошибке давления
        double mass_flow_rate = Kp * pressure_error * volume / (gas_const * temp);

        // Изменение массы за этот шаг
        double mass_change = mass_flow_rate * delta_time;

        // Ограничение скорости (безопасный предел)
        constexpr double MAX_FRACTION_PER_SEC = 0.05;
        double max_mass_change = state.get_mass() * MAX_FRACTION_PER_SEC * delta_time;
        mass_change = std::clamp(mass_change, -max_mass_change, max_mass_change);

        return mass_change;
    }
};

class HumidityController : public Controller {
public:
    HumidityController(double min, double max, bool control_state = true)
                     : Controller(Sensor({.min_value=min, .max_value=max}), control_state) {}

    template<typename T>
    [[nodiscard]] double get_value(T& state) { return std::max(std::min(state.get_humidity(), get_max_value()), get_min_value()); }
    [[nodiscard]] double get_min_value() { return get_sensor().get_min_value(); }
    [[nodiscard]] double get_max_value() { return get_sensor().get_max_value(); }

    template<typename T = State>
    static double calculate_water_injection_rate(T& state, double delta_time, double max_possible_mass) {
        double current = state.get_humidity();
        double needed = state.get_needed_humidity();
        double error = needed - current; // Если > 0, нужно увлажнять

        // Коэффициент пропорциональности. 
        // 0.1 означает: пытаемся исправить 10% ошибки за секунду.
        constexpr double Kp = 0.5; 

        // Желаемая скорость изменения влажности (% в секунду)
        double desired_humidity_change_speed = error * Kp;

        // Превращаем проценты в массу воды (kg/s)
        // Если мы хотим изменить влажность на 5%, нам нужно добавить 0.05 * max_mass воды.
        double needed_flow_rate = (desired_humidity_change_speed / 100.0) * max_possible_mass;

        // Физическое ограничение форсунки (Valve Limit).
        // Допустим, насос не может качать быстрее 0.05 кг/с (50 грамм в секунду)
        constexpr double MAX_PHYSICAL_FLOW = 0.05; 
        
        return std::clamp(needed_flow_rate, -MAX_PHYSICAL_FLOW, MAX_PHYSICAL_FLOW);
    }
};