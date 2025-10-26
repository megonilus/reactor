#pragma once
#include <algorithm>
#include <cmath>
#include <utility>

struct State;

struct PidParameters {
    double proportional_gain;
    double integral_gain;
    double derivative_gain;
    
    PidParameters() = default;
    
    PidParameters& set_proportional(double proportional) {
        proportional_gain = proportional;
        return *this;
    }
    
    PidParameters& set_integral(double integral) {
        integral_gain = integral;
        return *this;
    }
    
    PidParameters& set_derivative(double derivative) {
        derivative_gain = derivative;
        return *this;
    }
};

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
private:
    double integral_error = 0.0;
    double previous_error = 0.0;
    static constexpr double DEFAULT_KP = 1.0;
    static constexpr double DEFAULT_KI = 0.1;
    static constexpr double DEFAULT_KD = 0.01;
    static constexpr double COOLING_EFFICIENCY = 0.5;
    static constexpr double DEFAULT_PID_THRESHOLD = 20.0;
    static constexpr double INTEGRAL_WINDUP_LIMIT = 1000.0;
    static constexpr double DEFAULT_PARALLEL_THRESHOLD = 45.0;
    double kp = DEFAULT_KP;
    double ki = DEFAULT_KI;
    double kd = DEFAULT_KD;
    double startup_time = 0.0;
    double parallel_threshold = DEFAULT_PARALLEL_THRESHOLD;
    bool parallel_mode_enabled = true;
    
public:
    TemperatureController(double min, double max, bool control_state = true)
                        : Controller(Sensor({.min_value=min, .max_value=max}), control_state) {}

    template<typename T>
    [[nodiscard]] double get_value(T& state) { return std::max(std::min(state.get_temperature(), get_max_value()), get_min_value()); }
    [[nodiscard]] double get_min_value() { return get_sensor().get_min_value(); }
    [[nodiscard]] double get_max_value() { return get_sensor().get_max_value(); }
    
    template<typename T>
    double calculate_control_output(T& state, double delta_time) {
        double current_temperature = state.get_temperature();
        double target_temperature = state.get_needed_temperature();
        double error = target_temperature - current_temperature;
        
        static constexpr double STARTUP_DURATION = 5.0;
        double startup_factor = std::min(1.0, startup_time / STARTUP_DURATION);
        
        double proportional = kp * error;

        double proposed_integral = integral_error + (error * delta_time);
        if (std::abs(proposed_integral) > INTEGRAL_WINDUP_LIMIT) {
            proposed_integral = std::copysign(INTEGRAL_WINDUP_LIMIT, proposed_integral);
        }
        integral_error += error * delta_time;
        double integral = ki * integral_error;
        
        double derivative = kd * (previous_error - error) / delta_time;
        
        previous_error = error;
        
        double control_power = proportional + integral + derivative;
        
        control_power *= startup_factor;
        
        double max_heating_power = state.get_max_energy_consumption();
        double max_cooling_power = -max_heating_power * COOLING_EFFICIENCY;
        
        if (std::abs(error) <= parallel_threshold && parallel_mode_enabled) {
            double stabilization_factor = 1.0 - (std::abs(error) / parallel_threshold);
            
            if (control_power > 0) {
                double heating_power = std::min(control_power, max_heating_power);
                double cooling_power = std::min(max_cooling_power * 0.1 * stabilization_factor, 
                                               -std::abs(error) * 0.2 * stabilization_factor);
                return heating_power + cooling_power;
            }
            else {
                double cooling_power = std::max(control_power, max_cooling_power);
                double heating_power = std::min(max_heating_power * 0.1 * stabilization_factor, 
                                               std::abs(error) * 0.2 * stabilization_factor);
                return cooling_power + heating_power;
            }
        }
        else {
            double limited_power = std::max(max_cooling_power, std::min(control_power, max_heating_power));
            if (control_power != limited_power) {
                double back_calc = (limited_power - control_power) / kp;
                integral_error += back_calc * delta_time;
            }
            return limited_power;
        }
    }
    
    void set_pid_parameters(const PidParameters& params) {
        kp = params.proportional_gain;
        ki = params.integral_gain;
        kd = params.derivative_gain;
    }
    
    
    void update_startup_time(double delta_time) {
        startup_time += delta_time;
    }
    
    void set_parallel_mode(bool enabled) { parallel_mode_enabled = enabled; }
    [[nodiscard]] bool is_parallel_mode_enabled() const { return parallel_mode_enabled; }
    
    void set_parallel_threshold(double threshold) { parallel_threshold = threshold; }
    [[nodiscard]] double get_parallel_threshold() const { return parallel_threshold; }
    
    template<typename T>
    std::pair<double, double> calculate_parallel_control_output(T& state, double delta_time) {
        double current_temperature = state.get_temperature();
        double target_temperature = state.get_needed_temperature();
        double error = target_temperature - current_temperature;
        
        static constexpr double STARTUP_DURATION = 5.0;
        double startup_factor = std::min(1.0, startup_time / STARTUP_DURATION);
        
        double proportional = kp * error;

        double proposed_integral = integral_error + (error * delta_time);
        if (std::abs(proposed_integral) > INTEGRAL_WINDUP_LIMIT) {
            proposed_integral = std::copysign(INTEGRAL_WINDUP_LIMIT, proposed_integral);
        }
        integral_error += error * delta_time;
        double integral = ki * integral_error;
        
        double derivative = kd * (previous_error - error) / delta_time;
        
        previous_error = error;
        
        double control_power = proportional + integral + derivative;
        control_power *= startup_factor;
        
        double max_heating_power = state.get_max_energy_consumption();
        double max_cooling_power = -max_heating_power * COOLING_EFFICIENCY;
        
        if (std::abs(error) <= parallel_threshold && parallel_mode_enabled) {
            double heating_power = 0.0;
            double cooling_power = 0.0;
            
            double stabilization_factor = 1.0 - (std::abs(error) / parallel_threshold);
            
            if (control_power > 0) {
                heating_power = std::min(control_power, max_heating_power);
                cooling_power = std::min(max_cooling_power * 0.2 * stabilization_factor, 
                                       -std::abs(error) * 0.5 * stabilization_factor);
            }
            else {
                cooling_power = std::max(control_power, max_cooling_power);
                heating_power = std::min(max_heating_power * 0.2 * stabilization_factor, 
                                        std::abs(error) * 0.5 * stabilization_factor);
            }
            
            return {heating_power, -cooling_power};
        }
        else {
            double limited_power = std::max(max_cooling_power, std::min(control_power, max_heating_power));
            if (control_power != limited_power) {
                double back_calc = (limited_power - control_power) / kp;
                integral_error += back_calc * delta_time;
            }
            
            if (limited_power > 0) {
                return {limited_power, 0.0};
            }
            else {
                return {0.0, -limited_power};
            }
        }
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
};

class HumidityController : public Controller {
public:
    HumidityController(double min, double max, bool control_state = true)
                     : Controller(Sensor({.min_value=min, .max_value=max}), control_state) {}

    template<typename T>
    [[nodiscard]] double get_value(T& state) { return std::max(std::min(state.get_humidity(), get_max_value()), get_min_value()); }
    [[nodiscard]] double get_min_value() { return get_sensor().get_min_value(); }
    [[nodiscard]] double get_max_value() { return get_sensor().get_max_value(); }
};