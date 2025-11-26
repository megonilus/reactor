#pragma once
#include "../backend/backend.hpp"

#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>
#include <utility>

enum class ControlMode : std::uint8_t
{
	AUTOMATICLY,
	MANUAL
};

enum class StatusMode : std::uint8_t
{
	NORMAL,
	WARNING,
	CRITICAL
};

struct Environment
{
	double mass;				   // kg
	double volume;				   // m^3
	double temperature;			   // K
	double needed_temperature;	   // K
	double pressure;			   // Pa
	double needed_pressure;		   // Pa
	double humidity;			   // %
	double needed_humidity;		   // %
	double energy_consumption;	   // W
	double max_energy_consumption; // W

	double heat_capacity;			  // J/(kg·K) - удельная теплоемкость смеси
	double thermal_conductivity;	  // W/(m·K) - теплопроводность
	double surface_area;			  // m^2 - площадь поверхности реактора
	double wall_thickness;			  // m - толщина стенки
	double wall_thermal_conductivity; // W/(m·K) - теплопроводность стенки
	double ambient_temperature;		  // K - температура окружающей среды
	double heat_transfer_coefficient; // W/(m^2·K) - коэффициент теплопередачи
	double reaction_heat_rate;		  // W - скорость тепловыделения от реакций
	double cooling_rate;			  // W - скорость охлаждения
	double heating_rate;			  // W - скорость нагрева
	double specific_gas_constant;
};

struct State
{
private:
	Environment environment;

	ControlMode control_mode;
	StatusMode	status_mode = StatusMode::NORMAL;

	TemperatureController temp_controller;
	PressureController	  pressure_controller;
	HumidityController	  humidity_controller;

	std::atomic_bool running = false; // controls thread closing

public:
	State(Environment environment, ControlMode control_mode, TemperatureController temp_controller,
		  PressureController pressure_controller, HumidityController humidity_controller)
		: environment(environment),
		  control_mode(control_mode),
		  temp_controller(std::move(temp_controller)),
		  pressure_controller(std::move(pressure_controller)),
		  humidity_controller(std::move(humidity_controller))
	{
	}

	// Multithread safe access
	std::mutex mutex; // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes)

	[[nodiscard]] bool is_running() const
	{
		return running.load();
	}

	void set_running(bool new_value)
	{
		running.store(new_value);
	}

	[[nodiscard]] double get_mass() const
	{
		return environment.mass;
	}
	void set_mass(double mass)
	{
		environment.mass = mass;
	}

	[[nodiscard]] double get_volume() const
	{
		return environment.volume;
	}
	void set_volume(double volume)
	{
		environment.volume = volume;
	}

	[[nodiscard]] double get_specific_gas_constant() const
	{
		return environment.specific_gas_constant;
	}
	void set_specific_gas_constant(double specific_gas_constant)
	{
		environment.specific_gas_constant = specific_gas_constant;
	}

	[[nodiscard]] ControlMode get_control_mode() const
	{
		return control_mode;
	}

	void set_control_mode(ControlMode control_mode)
	{
		this->control_mode = control_mode;
	}
	[[nodiscard]] double get_temperature() const
	{
		return environment.temperature;
	}
	void set_temperature(double temperature)
	{
		environment.temperature = temperature;
	}

	[[nodiscard]] double get_needed_temperature() const
	{
		return environment.needed_temperature;
	}

	[[nodiscard]] double get_pressure() const
	{
		return environment.pressure;
	}
	void set_pressure(double pressure)
	{
		environment.pressure = pressure;
	}

	[[nodiscard]] double get_needed_pressure() const
	{
		return environment.needed_pressure;
	}

	[[nodiscard]] double get_humidity() const
	{
		return environment.humidity;
	}
	void set_humidity(double humidity)
	{
		environment.humidity = humidity;
	}

	[[nodiscard]] double get_needed_humidity() const
	{
		return environment.needed_humidity;
	}

	[[nodiscard]] double get_energy_consumption() const
	{
		return environment.energy_consumption;
	}
	void set_energy_consumption(double energy_consumption)
	{
		environment.energy_consumption = energy_consumption;
	}

	[[nodiscard]] double get_max_energy_consumption() const
	{
		return environment.max_energy_consumption;
	}

	[[nodiscard]] double get_heat_capacity() const
	{
		return environment.heat_capacity;
	}
	void set_heat_capacity(double heat_capacity)
	{
		environment.heat_capacity = heat_capacity;
	}

	[[nodiscard]] double get_thermal_conductivity() const
	{
		return environment.thermal_conductivity;
	}
	void set_thermal_conductivity(double thermal_conductivity)
	{
		environment.thermal_conductivity = thermal_conductivity;
	}

	[[nodiscard]] double get_surface_area() const
	{
		return environment.surface_area;
	}
	void set_surface_area(double surface_area)
	{
		environment.surface_area = surface_area;
	}

	[[nodiscard]] double get_wall_thickness() const
	{
		return environment.wall_thickness;
	}
	void set_wall_thickness(double wall_thickness)
	{
		environment.wall_thickness = wall_thickness;
	}

	[[nodiscard]] double get_wall_thermal_conductivity() const
	{
		return environment.wall_thermal_conductivity;
	}
	void set_wall_thermal_conductivity(double wall_thermal_conductivity)
	{
		environment.wall_thermal_conductivity = wall_thermal_conductivity;
	}

	[[nodiscard]] double get_ambient_temperature() const
	{
		return environment.ambient_temperature;
	}
	void set_ambient_temperature(double ambient_temperature)
	{
		environment.ambient_temperature = ambient_temperature;
	}

	[[nodiscard]] double get_heat_transfer_coefficient() const
	{
		return environment.heat_transfer_coefficient;
	}
	void set_heat_transfer_coefficient(double heat_transfer_coefficient)
	{
		environment.heat_transfer_coefficient = heat_transfer_coefficient;
	}

	[[nodiscard]] double get_reaction_heat_rate() const
	{
		return environment.reaction_heat_rate;
	}
	void set_reaction_heat_rate(double reaction_heat_rate)
	{
		environment.reaction_heat_rate = reaction_heat_rate;
	}

	[[nodiscard]] double get_cooling_rate() const
	{
		return environment.cooling_rate;
	}
	void set_cooling_rate(double cooling_rate)
	{
		environment.cooling_rate = cooling_rate;
	}

	[[nodiscard]] double get_heating_rate() const
	{
		return environment.heating_rate;
	}
	void set_heating_rate(double heating_rate)
	{
		environment.heating_rate = heating_rate;
	}

	[[nodiscard]] StatusMode get_status_mode() const
	{
		return status_mode;
	}
	void set_status_mode(StatusMode status_mode)
	{
		this->status_mode = status_mode;
	}
};
