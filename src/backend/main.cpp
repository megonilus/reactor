#include "../../includes/simulation/simulation.hpp"
#include <chrono>
#include <thread>

namespace {
    constexpr double MASS = 230.0;
    constexpr double VOLUME = 1.0;
    constexpr double TEMP = 400.0;
    constexpr double NEEDED_TEMP = 350.0;
    constexpr double PRESSURE = 101325.0;
    constexpr double NEEDED_PRESSURE = 1500000.0;
    constexpr double HUMIDITY = 50.0;
    constexpr double NEEDED_HUMIDITY = 30.0;
    constexpr double ENERGY_CONSUMPTION = 1000.0;
    constexpr double MAX_ENERGY_CONSUMPTION = 1200000.0;
    constexpr double MIN_TEMP = 150.0;
    constexpr double MAX_TEMP = 500.0;
    constexpr double MAX_PRESSURE = 1000000.0;
    constexpr double MAX_HUMIDITY = 100.0;
    constexpr int TIME_OF_TICK = 100;

    constexpr double HEAT_CAPACITY = 4180.0;
    constexpr double THERMAL_CONDUCTIVITY = 0.6;
    constexpr double SURFACE_AREA = 0.5;
    constexpr double WALL_THICKNESS = 0.2;
    constexpr double WALL_THERMAL_CONDUCTIVITY = 0.005;
    constexpr double AMBIENT_TEMPERATURE = 293.0;
    constexpr double HEAT_TRANSFER_COEFFICIENT = 0.05;
    constexpr double REACTION_HEAT_RATE = 0.0;
    constexpr double COOLING_RATE = 0.0;
    constexpr double HEATING_RATE = 15000.0;
	constexpr double SPECIFIC_GAS_CONSTANT = 287.0;
}

int main()
{
	Environment env = {
		.mass = MASS,
		.volume = VOLUME,
		.temperature = TEMP,
		.needed_temperature = NEEDED_TEMP,
		.pressure = PRESSURE,
		.needed_pressure = NEEDED_PRESSURE,
		.humidity = HUMIDITY,
		.needed_humidity = NEEDED_HUMIDITY,
		.energy_consumption = ENERGY_CONSUMPTION,
		.max_energy_consumption = MAX_ENERGY_CONSUMPTION,
		.heat_capacity = HEAT_CAPACITY,
		.thermal_conductivity = THERMAL_CONDUCTIVITY,
		.surface_area = SURFACE_AREA,
		.wall_thickness = WALL_THICKNESS,
		.wall_thermal_conductivity = WALL_THERMAL_CONDUCTIVITY,
		.ambient_temperature = AMBIENT_TEMPERATURE,
		.heat_transfer_coefficient = HEAT_TRANSFER_COEFFICIENT,
		.reaction_heat_rate = REACTION_HEAT_RATE,
		.cooling_rate = COOLING_RATE,
		.heating_rate = HEATING_RATE,
		.specific_gas_constant = SPECIFIC_GAS_CONSTANT
	};
	
	Simulation simulation(env, MIN_TEMP, MAX_TEMP, 0, MAX_PRESSURE, 0, MAX_HUMIDITY);

	auto previous_time = std::chrono::high_resolution_clock::now();
	while (simulation.get_simulation_flag()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(TIME_OF_TICK));
		
        auto current_time = std::chrono::high_resolution_clock::now();
		
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - previous_time);
		simulation.simulate(duration.count());

		previous_time = current_time;
	}
	return 0;
}
