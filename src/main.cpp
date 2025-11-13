#include "common.hpp"

#include <thread>

using std::thread;

extern void	  start_simulation();
extern void	  render_tui(State* state);
extern State* get_state();

int main(int argc, char* argv[])
{
	State* current_state = get_state();
	thread simulation_thread(start_simulation);
	thread tui_thread(render_tui, current_state);

	tui_thread.join();

	{
		std::scoped_lock lock(current_state->mutex);
		current_state->set_running(false);
	}

	simulation_thread.join();
	return 0;
}
