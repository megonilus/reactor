#pragma once
#include <algorithm>

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
    Controller(Controller&& other) noexcept;
    Controller& operator =(Controller&& other) = default;
    virtual ~Controller() = default;

    [[nodiscard]] Sensor get_sensor() { return sensor; }
    [[nodiscard]] bool have_control() const { return control_state; }
    void set_control(bool state) { control_state = state; }
    virtual void control(State& state);
};

class TemperatureController : public Controller {
public:
    TemperatureController(double min, double max, bool control_state = true)
                        : Controller(Sensor({.min_value=min, .max_value=max}), control_state) {}

    template<typename T>
    [[nodiscard]] double get_value(T& state) { return std::max(std::min(state.get_temperature(), get_max_value()), get_min_value()); }
    [[nodiscard]] double get_min_value() { return get_sensor().get_min_value(); }
    [[nodiscard]] double get_max_value() { return get_sensor().get_max_value(); }

    void control(State& state) override {

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

    void control(State& state) override {

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

    void control(State& state) override {
        
    }
};