#include "../../includes/simulation/simulation.hpp"

#include <chrono>
#include <memory>
#include <thread>

std::shared_ptr<Simulation>
	simulation; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

// void start_simulation()
// {
// 	simulation =
// 		std::make_shared<Simulation>(ENV, MIN_TEMP, MAX_TEMP, 0, MAX_PRESSURE, 0, MAX_HUMIDITY);
//
// 	auto previous_time = std::chrono::high_resolution_clock::now();
// 	while (simulation->get_simulation_flag())
// 	{
// 		std::this_thread::sleep_for(std::chrono::milliseconds(TIME_OF_TICK));
//
// 		auto current_time = std::chrono::high_resolution_clock::now();
//
// 		auto duration =
// 			std::chrono::duration_cast<std::chrono::milliseconds>(current_time - previous_time);
// 		simulation->simulate(duration.count());
// 	}
// }

void Simulation::operator()()
{
	auto previous_time = std::chrono::high_resolution_clock::now();
	while (state.is_running())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(TIME_OF_TICK));
		auto current_time = std::chrono::high_resolution_clock::now();
		auto duration =
			std::chrono::duration_cast<std::chrono::milliseconds>(current_time - previous_time);
		simulate(duration.count());
		previous_time = current_time;
	}
}
