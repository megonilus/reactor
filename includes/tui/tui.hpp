#pragma once

#include <common.hpp>
#include <connect.hpp>
#include <ftxui/component/component_base.hpp>

namespace tui
{
	class MainWindow
	{
		std::string test_output;

	public:
		MainWindow(const MainWindow&)			 = default;
		MainWindow(MainWindow&&)				 = default;
		MainWindow& operator=(const MainWindow&) = default;
		MainWindow& operator=(MainWindow&&)		 = default;
		~MainWindow()							 = default;

		MainWindow() : test_output("Main Window") {}

		ftxui::Component renderer();
	};

	class ControlWindow
	{
		std::string test_output;

	public:
		ControlWindow(const ControlWindow&)			   = default;
		ControlWindow(ControlWindow&&)				   = default;
		ControlWindow& operator=(const ControlWindow&) = default;
		ControlWindow& operator=(ControlWindow&&)	   = default;
		~ControlWindow()							   = default;

		explicit ControlWindow(std::string test_output) : test_output(std::move(test_output)) {}
		ControlWindow() : test_output("Control Window") {}

		ftxui::Component renderer();
	};

	class StatWindow
	{
		std::string test_output;

	public:
		StatWindow(const StatWindow&)			 = default;
		StatWindow(StatWindow&&)				 = default;
		StatWindow& operator=(const StatWindow&) = default;
		StatWindow& operator=(StatWindow&&)		 = default;
		~StatWindow()							 = default;

		explicit StatWindow(std::string test_output) : test_output(std::move(test_output)) {}
		StatWindow() : test_output("Stat Window") {}

		ftxui::Component renderer();
	};

	class Bar
	{

		std::vector<std::string> tab_names;
		int						 tab_selected = 0;

		MainWindow	  main_window;
		ControlWindow control_window;
		StatWindow	  stat_window;

	public:
		ftxui::Component renderer();

		Bar(const Bar&)			   = default;
		Bar(Bar&&)				   = default;
		Bar& operator=(const Bar&) = default;
		Bar& operator=(Bar&&)	   = default;
		~Bar()					   = default;

		explicit Bar(std::vector<std::string> tab_names) : tab_names(std::move(tab_names)) {}
		explicit Bar() : tab_names({"main", "control", "stat"}) {};
	};

	class Instance
	{
		std::string name;
		ConnectPtr	connection;

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
