#include "common.hpp"

#include <thread>

using std::thread;

extern void start_simulation();
extern void render_tui(SafeState state);

int main(int argc, char* argv[])
{
	thread simulation_thread(start_simulation);

	{
		std::scoped_lock lock(state.mutex);
	}

	return 0;
}
