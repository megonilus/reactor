#pragma once
#include "../backend/backend.hpp"
#include "../common/common.hpp"
#include "thermodynamics.hpp"

namespace
{
	constexpr double MASS					= 100.0;
	constexpr double VOLUME					= 1.0;
	constexpr double TEMP					= 200.0;
	constexpr double NEEDED_TEMP			= 450.0;
	constexpr double PRESSURE				= 101325.0;
	constexpr double NEEDED_PRESSURE		= 150000.0;
	constexpr double HUMIDITY				= 50.0;
	constexpr double NEEDED_HUMIDITY		= 30.0;
	constexpr double ENERGY_CONSUMPTION		= 1000.0;
	constexpr double MAX_ENERGY_CONSUMPTION = 20000.0;
	constexpr double MIN_TEMP				= 273.0;
	constexpr double MAX_TEMP				= 500.0;
	constexpr double MAX_PRESSURE			= 1000000.0;
	constexpr double MAX_HUMIDITY			= 100.0;
	constexpr int	 TIME_OF_TICK			= 100;

	constexpr double HEAT_CAPACITY			   = 4180.0;
	constexpr double THERMAL_CONDUCTIVITY	   = 0.6;
	constexpr double SURFACE_AREA			   = 0.5;
	constexpr double WALL_THICKNESS			   = 0.2;
	constexpr double WALL_THERMAL_CONDUCTIVITY = 0.005;
	constexpr double AMBIENT_TEMPERATURE	   = 293.0;
	constexpr double HEAT_TRANSFER_COEFFICIENT = 0.05;
	constexpr double REACTION_HEAT_RATE		   = 0.0;
	constexpr double COOLING_RATE			   = 0.0;
	constexpr double HEATING_RATE			   = 15000.0;
	constexpr double SPECIFIC_GAS_CONSTANT	   = 287.0;
} // namespace

const Environment ENV = {.mass						= MASS,
						 .volume					= VOLUME,
						 .temperature				= TEMP,
						 .needed_temperature		= NEEDED_TEMP,
						 .pressure					= PRESSURE,
						 .needed_pressure			= NEEDED_PRESSURE,
						 .humidity					= HUMIDITY,
						 .needed_humidity			= NEEDED_HUMIDITY,
						 .energy_consumption		= ENERGY_CONSUMPTION,
						 .max_energy_consumption	= MAX_ENERGY_CONSUMPTION,
						 .heat_capacity				= HEAT_CAPACITY,
						 .thermal_conductivity		= THERMAL_CONDUCTIVITY,
						 .surface_area				= SURFACE_AREA,
						 .wall_thickness			= WALL_THICKNESS,
						 .wall_thermal_conductivity = WALL_THERMAL_CONDUCTIVITY,
						 .ambient_temperature		= AMBIENT_TEMPERATURE,
						 .heat_transfer_coefficient = HEAT_TRANSFER_COEFFICIENT,
						 .reaction_heat_rate		= REACTION_HEAT_RATE,
						 .cooling_rate				= COOLING_RATE,
						 .heating_rate				= HEATING_RATE,
						 .specific_gas_constant		= SPECIFIC_GAS_CONSTANT};

class Simulation
{
private:
	TemperatureController temp_controller;
	PressureController	  pressure_controller;
	HumidityController	  humidity_controller;
	unsigned long		  current_time_millis = 0;

public:
	State state; // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes)

	Simulation(Environment env, const double MIN_TEMP, const double MAX_TEMP,
			   const double MIN_PRESSURE, const double MAX_PRESSURE, const double MIN_HUMIDITY,
			   const double MAX_HUMIDITY)
		: temp_controller(MIN_TEMP, MAX_TEMP),
		  pressure_controller(MIN_PRESSURE, MAX_PRESSURE),
		  humidity_controller(MIN_HUMIDITY, MAX_HUMIDITY),
		  state(env, ControlMode::AUTOMATICLY, TemperatureController(MIN_TEMP, MAX_TEMP),
				PressureController(MIN_PRESSURE, MAX_PRESSURE),
				HumidityController(MIN_HUMIDITY, MAX_HUMIDITY))
	{
	}

	void operator()();

	void simulate(unsigned long milliseconds)
	{
		if (!state.is_running())
		{
			return;
		}

		const unsigned long MILLIS_IN_SEC = 1000;
		double				d_t			  = (double) milliseconds / MILLIS_IN_SEC;
		current_time_millis += milliseconds;

		if (d_t <= 0.0)
		{
			return;
		}

		Thermodynamics::update_temperature_with_controller(state, d_t);
		Thermodynamics::update_pressure_with_controller(state, d_t);

		// std::cout << "Время: " << current_time_millis / MILLIS_IN_SEC << " с,\n"
		// 		  << "Температура: " << state.get_temperature() << " K,\n"
		// 		  << "Целевая температура: " << state.get_needed_temperature() << " K,\n"
		// 		  << "Давление: " << state.get_pressure() << " Pa,\n"
		// 		  << "Целевое давление: " << state.get_needed_pressure() << " Pa,\n"
		// 		  << "Масса: " << state.get_mass() << " kg,\n"
		// 		  << "Теплоемкость: " << state.get_heat_capacity() << " J/(kg·K),\n"
		// 		  << "Тепловыделение: " << state.get_reaction_heat_rate() << " W,\n"
		// 		  << "Нагрев: " << state.get_heating_rate() << " W,\n"
		// 		  << "Охлаждение: " << state.get_cooling_rate() << " W\n\n";
	}

	static std::shared_ptr<Simulation> shared_simulation()
	{
		return std::make_shared<Simulation>(ENV, MIN_TEMP, MAX_TEMP, 0, MAX_PRESSURE, 0,
											MAX_HUMIDITY);
	}
};

using SharedSimulation = std::shared_ptr<Simulation>;
