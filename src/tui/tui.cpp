#include "common.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/flexbox_config.hpp>
#include <tui.hpp>

using namespace tui;
using namespace ftxui;
using namespace std::chrono_literals;

void render_tui(State* state)
{
	Instance instance(state);
	instance.display();
}

void Instance::display()
{
	auto screen = ScreenInteractive::Fullscreen();

	Bar	 bar(state);
	auto bar_renderer = bar.component();

	auto root = Renderer(bar_renderer,
						 [&screen, bar_renderer]
						 {
							 screen.RequestAnimationFrame();
							 return vbox({bar_renderer->Render()}) | border;
						 });

	std::thread sleeper(
		[]
		{
			while (true)
			{
				std::this_thread::sleep_for(50ms);
			}
		});

	screen.Loop(root);

	sleeper.detach();
}

Component Bar::component()
{
	auto tab_toggle = Toggle(&tab_names, &tab_selected);

	FlexboxConfig config;
	config.direction = FlexboxConfig::Direction::Column;

	auto tab_container = Container::Tab({main_component, stat_component}, &tab_selected);

	auto tab_filled = Renderer(tab_container, [tab_container]
							   { return tab_container->Render() | yflex | xflex | frame; });

	auto container = Container::Vertical({
						 tab_toggle,
						 Renderer([] { return separator(); }),
						 tab_filled,
					 }) |
					 flex;

	return container;
}

Component MainWindow::component()
{
	auto btn_toggle_sim = Button(
		"Toggle Simulation",
		[this]
		{
			bool current = state->is_running();
			state->set_running(!current);
		},
		ButtonOption::Ascii());

	auto controls_container = Container::Vertical({
		btn_toggle_sim,
	});

	auto info_component_wrapper = Renderer([this] { return info.element(); });

	auto main_layout = Container::Horizontal({
		info_component_wrapper,
		controls_container,
	});
	return Renderer(main_layout,
					[this, btn_toggle_sim]
					{
						bool is_running = state->is_running();

						auto status_text =
							is_running ? text(" SIMULATION: RUNNING ") | bold | color(Color::Green)
									   : text(" SIMULATION: STOPPED ") | bold | color(Color::Red);

						auto right_panel = vbox({
											   text("REACTOR CONTROL") | hcenter | bold,
											   separator(),
											   status_text | hcenter | border,
											   filler(),
											   btn_toggle_sim->Render() | center,
											   filler(),
										   }) |
										   border;

						return hbox({info.element() | flex, separator(), right_panel | flex});
					});
};

Component StatWindow::component()
{
	return Renderer(
		[this]
		{
			indicators.get_content().rerender_all();

			return indicators.element();
		});
}
