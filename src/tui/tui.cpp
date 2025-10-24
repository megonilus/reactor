#include <cstdint>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/flexbox_config.hpp>
#include <tui.hpp>

using namespace tui;
using namespace ftxui;

void Instance::display()
{
	auto screen = ScreenInteractive::Fullscreen();

	Bar	 bar;
	auto bar_renderer = bar.renderer();

	auto root = Renderer(bar_renderer, [&] { return vbox({bar_renderer->Render()}) | border; });

	screen.Loop(root);
}

Component Bar::renderer()
{
	auto tab_toggle = Toggle(&tab_names, &tab_selected);

	FlexboxConfig config;
	config.direction = FlexboxConfig::Direction::Column;

	auto tab_container = Container::Tab(
		{main_window.renderer() | yflex_grow, control_window.renderer(), stat_window.renderer()

		},
		&tab_selected);

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
Component MainWindow::renderer()
{

	// constexpr uint16_t CONTAINER_WIDTH = 500;
	// constexpr uint8_t  CONTAINER_HEIGHT = 15;
	// constexpr uint8_t  WINDOW_RATIO		= CONTAINER_WIDTH / 2;

	auto info_content		= text("Info content");
	auto connection_content = text("Connection info ");

	auto info2_content	 = text("Info 2 content");
	auto reactor_content = text("Reactor content");

	FlexboxConfig config;
	config.direction	   = FlexboxConfig::Direction::Row;
	config.align_content   = FlexboxConfig::AlignContent::Center;
	config.align_items	   = FlexboxConfig::AlignItems::Stretch;
	config.justify_content = FlexboxConfig::JustifyContent::Center;

	FlexboxConfig col_config;
	col_config.direction	   = FlexboxConfig::Direction::Column;
	col_config.justify_content = FlexboxConfig::JustifyContent::Stretch;
	col_config.align_items	   = FlexboxConfig::AlignItems::Stretch;
	col_config.align_content   = FlexboxConfig::AlignContent::Stretch;

	auto top = flexbox(
		{
			window(text("Info"), info_content) | flex,
			window(text("Connection"), connection_content) | flex,
		},
		config);

	auto bot = flexbox({window(text("Info 2"), info2_content) | flex,
						window(text("Reactor state"), reactor_content) | flex},
					   config);

	auto layout = flexbox({top | flex, bot | flex}, col_config);

	return Renderer([layout] { return layout; });
};

Component StatWindow::renderer()
{

	return Container::Vertical({
		Renderer([this] { return text(test_output) | border; }),
	});
}

Component ControlWindow::renderer()
{

	return Container::Vertical({
		Renderer([this] { return text(test_output) | border; }),
	});
}
