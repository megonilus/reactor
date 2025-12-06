#pragma once
#include <string>
#include "../../external/tomlplusplus/toml.hpp"

constexpr double WALL_THERMAL_CONDUCTIVITY = 0.005;
constexpr double AMBIENT_TEMPERATURE	   = 293.0;
constexpr double HEAT_TRANSFER_COEFFICIENT = 0.05;
constexpr double REACTION_HEAT_RATE		   = 0.0;
constexpr double COOLING_RATE			   = 0.0;
constexpr double HEATING_RATE			   = 15000.0;
constexpr double SPECIFIC_GAS_CONSTANT	   = 287.0;

struct ReactorConfig
{
	double surface_area{};
	struct
	{
		double thickness{};
		double thermal_conductivity = WALL_THERMAL_CONDUCTIVITY;
	} wall;
};

struct MassConfig
{
	double input{};
	double output{};
};

struct ReactionConfig
{
	// goal
	double needed_temp{};
	double needed_humidity{};
	double needed_pressure{};
	double volume{};

	// start values
	double pressure{};
	double humidity{};
	double temperature{};

	// energy
	struct
	{
		double consumption;
		double max_consumption;
	} energy{};

	// constants
	// -- defaulted constants
	double ambient_temperature		 = AMBIENT_TEMPERATURE;
	double specific_gas_constant	 = SPECIFIC_GAS_CONSTANT;
	double heat_transfer_coefficient = HEAT_TRANSFER_COEFFICIENT;
	double cooling_rate				 = COOLING_RATE;
	double heating_rate				 = HEATING_RATE;

	double heat_capacity{};
	double thermal_conductivity{};

	double min_temp{};
	double max_temp{};
	double max_pressure{};
	double max_humidity{};
};

struct AppConfig
{
	ReactorConfig  reactor{};
	MassConfig	   mass{};
	ReactionConfig reaction{};
};

namespace cfg
{
	/// Load the config.
	[[nodiscard]] AppConfig load_config(const std::string& path);
	std::string				config_path();
} // namespace cfg
