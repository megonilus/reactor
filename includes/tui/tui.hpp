#pragma once

#include <connect.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/string.hpp>

namespace tui
{
	class Instance
	{
		std::string name;
		ConnectPtr	connection;

		void display_bar();

	public:
		Instance(std::string name, ConnectPtr connection)
			: name(std::move(name)), connection(std::move(connection))
		{
		}
		Instance(const Instance&)			 = delete;
		Instance(Instance&&)				 = default;
		Instance& operator=(const Instance&) = delete;
		Instance& operator=(Instance&&)		 = default;
		~Instance()							 = default;

		// runs and displays
		// static for now
		static void display();
	};
} // namespace tui
