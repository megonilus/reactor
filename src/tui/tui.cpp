#include "common.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/flexbox_config.hpp>
#include <tui.hpp>

using namespace tui;
using namespace ftxui;

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

	auto root =
		Renderer(bar_renderer, [bar_renderer] { return vbox({bar_renderer->Render()}) | border; });

	screen.Loop(root);
}

Component Bar::component()
{
	auto tab_toggle = Toggle(&tab_names, &tab_selected);

	FlexboxConfig config;
	config.direction = FlexboxConfig::Direction::Column;

	auto tab_container =
		Container::Tab({main_component, control_component, stat_component}, &tab_selected);

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

	// constexpr uint16_t CONTAINER_WIDTH = 500;
	// constexpr uint8_t  CONTAINER_HEIGHT = 15;
	// constexpr uint8_t  WINDOW_RATIO		= CONTAINER_WIDTH / 2;

	FlexboxConfig config;
	config.direction	   = FlexboxConfig::Direction::Row;
	config.align_content   = FlexboxConfig::AlignContent::Center;
	config.align_items	   = FlexboxConfig::AlignItems::Stretch;
	config.justify_content = FlexboxConfig::JustifyContent::Stretch;

	FlexboxConfig col_config;
	col_config.direction	   = FlexboxConfig::Direction::Column;
	col_config.justify_content = FlexboxConfig::JustifyContent::Stretch;
	col_config.align_items	   = FlexboxConfig::AlignItems::Stretch;
	col_config.align_content   = FlexboxConfig::AlignContent::Stretch;

	auto top = flexbox({info.element(), connect_info.element()}, config);

	auto bot = flexbox({authors.element(), reactor_state_min.element()}, config);

	return Renderer([top, bot, col_config]
					{ return flexbox({top | flex, bot | flex}, col_config); });
};

Component StatWindow::component()
{
	return Renderer([this] { return temp.element(); });
}

Component ControlWindow::component()
{
	return Renderer([] { return text("Controls") | border; });
}
