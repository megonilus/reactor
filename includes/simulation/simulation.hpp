#pragma once
#include "../backend/backend.hpp"
#include "../common/common.hpp"
#include "../config/config.hpp"
#include "thermodynamics.hpp"

#include <memory>

constexpr int TIME_OF_TICK = 100;

const static AppConfig CFG = cfg::load_config(cfg::config_path()); // NOLINT(cert-err58-cpp)

inline Environment make_environment(const AppConfig& cfg)
{
	const auto& reaction = cfg.reaction;
	const auto& mass	 = cfg.mass;
	const auto& reactor	 = cfg.reactor;

	return Environment{
		.mass	= mass.input,	   // kg
		.volume = reaction.volume, // m^3

		.temperature		= reaction.temperature, // K
		.needed_temperature = reaction.needed_temp, // K

		.pressure		 = reaction.pressure,		 // Pa
		.needed_pressure = reaction.needed_pressure, // Pa

		.humidity		 = reaction.humidity,		 // %
		.needed_humidity = reaction.needed_humidity, // %

		.energy_consumption		= reaction.energy.consumption,	   // W
		.max_energy_consumption = reaction.energy.max_consumption, // W

		.heat_capacity		  = reaction.heat_capacity,		   // J/(kg·K)
		.thermal_conductivity = reaction.thermal_conductivity, // W/(m·K)

		.surface_area			   = reactor.surface_area,				// m²
		.wall_thickness			   = reactor.wall.thickness,			// m
		.wall_thermal_conductivity = reactor.wall.thermal_conductivity, // W/(m·K)

		.ambient_temperature	   = reaction.ambient_temperature,		 // K
		.heat_transfer_coefficient = reaction.heat_transfer_coefficient, // W/(m²·K)

		.reaction_heat_rate = REACTION_HEAT_RATE,
		.cooling_rate		= reaction.cooling_rate, // W
		.heating_rate		= reaction.heating_rate, // W

		.specific_gas_constant = reaction.specific_gas_constant,
	};
}

const Environment ENV = make_environment(CFG); // NOLINT(cert-err58-cpp)

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

		// 1. Контроллер влажности меняет массу (добавляет воду) и температуру (испарение).
		Thermodynamics::update_humidity_with_controller(state, d_t);

		// 2. Контроллер температуры компенсирует потери тепла
		Thermodynamics::update_temperature_with_controller(state, d_t);

		// 3. Контроллер давления реагирует на изменение общей массы и температуры (PV=nRT).
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
		return std::make_shared<Simulation>(ENV, CFG.reaction.min_temp, CFG.reaction.max_temp, 0,
											CFG.reaction.max_pressure, 0,
											CFG.reaction.max_humidity);
	}
};

using SharedSimulation = std::shared_ptr<Simulation>;
