#include <ftxui/screen/screen.hpp>
#include <tui.hpp>

using namespace tui;
using namespace ftxui;

void Instance::display()
{
	Element document = hbox({
		text("Hello, World!") | border,
	});
	auto	screen	 = Screen::Create(Dimension::Full(), Dimension::Fit(document));

	Render(screen, document);
	screen.Print();
}
