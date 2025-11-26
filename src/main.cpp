#include "../includes/simulation/simulation.hpp"
#include "common.hpp"

#include <thread>

using std::thread;

extern void render_tui(State* state);

int main()
{
	SharedSimulation simulation	   = Simulation::shared_simulation();
	State*			 current_state = &simulation->state;

	thread simulation_thread(&Simulation::operator(), simulation);
	thread tui_thread(render_tui, current_state);

	tui_thread.join();

	current_state->set_terminated(true);
	simulation_thread.join();

	return 0;
}
