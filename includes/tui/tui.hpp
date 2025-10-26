#pragma once

#include <common.hpp>
#include <connect.hpp>
#include <cstdint>
#include <ftxui/component/component_base.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/flexbox_config.hpp>
#include <ftxui/dom/node.hpp>
#include <ftxui/screen/color.hpp>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <variant>
#include <vector>

#ifndef PROJECT_NAME
#define PROJECT_NAME "unknown"
#endif

#ifndef PROJECT_VERSION
#define PROJECT_VERSION "0.0.0"
#endif

#ifndef COMPILER_INFO
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#if defined(__clang__)
#define COMPILER_INFO "Clang " STR(__clang_major__) "." STR(__clang_minor__)
#elif defined(__GNUC__)
#define COMPILER_INFO "GCC " STR(__GNUC__) "." STR(__GNUC_MINOR__)
#elif defined(_MSC_VER)
#define COMPILER_INFO "MSVC " STR(_MSC_VER)
#else
#define COMPILER_INFO "Unknown"
#endif
#endif

#define BUILD_DATE __DATE__
#define BUILD_TIME __TIME__
#define BUILD_MODE (#ifdef NDEBUG "Release" #else "Debug" #endif)

namespace tui
{
	using FieldValue = std::variant<std::string, double>;
	using Key		 = std::string;
	using Color		 = std::tuple<uint8_t, uint8_t, uint8_t>;

	class Window
	{
		static constexpr uint8_t DEFAULT_R = 60;
		static constexpr uint8_t DEFAULT_G = 179;
		static constexpr uint8_t DEFAULT_B = 113;

		std::string name  = "window";
		Color		color = std::make_tuple(DEFAULT_R, DEFAULT_G, DEFAULT_B);

	public:
		Window(const Window&)			 = default;
		Window(Window&&)				 = default;
		Window& operator=(const Window&) = default;
		Window& operator=(Window&&)		 = default;
		virtual ~Window()				 = default;
		Window()						 = default;

		virtual ftxui::Component component()
		{
			return ftxui::Renderer(
				[this]
				{
					return ftxui::window(ftxui::text(name) |
											 ftxui::color(ftxui::Color::RGB(
												 get<0>(color), get<1>(color), get<2>(color))),
										 ftxui::text("test" + name));
				});
		}

		[[nodiscard]] const std::string& get_name() const
		{
			return name;
		}

		void set_name(const std::string& new_name)
		{
			name = new_name;
		}

		void set_color(const Color& new_color)
		{
			color = new_color;
		}
	};

	class BaseField
	{
		Key			   key;
		ftxui::Element cached_element = ftxui::text("");
		bool		   dirty		  = true;

	protected:
		virtual void update_value_impl(const FieldValue& value)
		{
			if (std::holds_alternative<std::string>(value))
			{
				cached_element = ftxui::text(std::get<std::string>(value));
			}
		};

		virtual ftxui::Element renderer_impl()
		{
			return ftxui::text("");
		};

	public:
		BaseField(const BaseField&)			   = default;
		BaseField(BaseField&&)				   = delete;
		BaseField& operator=(const BaseField&) = default;
		BaseField& operator=(BaseField&&)	   = delete;
		explicit BaseField(Key key) : key(std::move(key)) {}
		virtual ~BaseField() = default;

		void update_value(const FieldValue& value)
		{
			dirty = true;
			update_value_impl(value);
		}

		ftxui::Element renderer()
		{
			if (dirty)
			{
				auto element   = renderer_impl();
				dirty		   = false;
				cached_element = element;
			}

			return cached_element;
		}

		[[nodiscard]] Key get_key() const noexcept
		{
			return key;
		}
	};

	// key - value pair
	struct KeyValuePair
	{
		Key			key;
		std::string val;
	};

	class LinkField : public BaseField
	{
		std::string value;
		std::string link;

	protected:
		void update_value_impl(const FieldValue& val) override
		{
			if (std::holds_alternative<std::string>(val))
			{
				value = std::get<std::string>(val);
			}
			else
			{
				// if double then use precision 2
				std::ostringstream oss;
				oss << std::fixed << std::setprecision(2) << std::get<double>(val);
				value = oss.str();
			}
		}

		ftxui::Element renderer_impl() override
		{
			constexpr uint8_t DEFAULT_LINK_G = 102;
			constexpr uint8_t DEFAULT_LINK_B = 204;

			return ftxui::hbox(
				{ftxui::text(get_key()), ftxui::filler(),
				 ftxui::hyperlink(link, ftxui::text(value)) |
					 ftxui::color(ftxui::Color::RGB(0, DEFAULT_LINK_G, DEFAULT_LINK_B))});
		}

	public:
		struct LinkData
		{
			Key			key;
			std::string val;
			std::string link;
		};
		explicit LinkField(const KeyValuePair& pair)
			: BaseField(pair.key), value(pair.val), link(pair.val)
		{
		}

		explicit LinkField(const LinkData& data)
			: BaseField(data.key), value(data.val), link(data.link)
		{
		}
	};

	class TextField : public BaseField
	{
		std::string value;

	protected:
		void update_value_impl(const FieldValue& val) override
		{
			if (std::holds_alternative<std::string>(val))
			{
				value = std::get<std::string>(val);
			}
			else
			{
				// if double then use precision 2
				std::ostringstream oss;
				oss << std::fixed << std::setprecision(2) << std::get<double>(val);
				value = oss.str();
			}
		}

		ftxui::Element renderer_impl() override
		{
			return ftxui::hbox({ftxui::text(get_key()), ftxui::filler(), ftxui::text(value)});
		}

	public:
		TextField(const TextField&)			   = default;
		TextField(TextField&&)				   = delete;
		TextField& operator=(const TextField&) = default;
		TextField& operator=(TextField&&)	   = delete;
		~TextField() override				   = default;

		explicit TextField(const KeyValuePair& val) : BaseField(val.key), value(val.val) {}
	};

	using Field	 = std::unique_ptr<BaseField>;
	using Fields = std::vector<Field>;

	struct Content
	{
		Fields										fields;
		std::unordered_map<std::string, BaseField*> index; // lookup by id

		void add(Field field)
		{
			index[field->get_key()] = field.get();
			fields.push_back(std::move(field));
		}

		void update_by_key(const Key& key, const FieldValue& value)
		{
			auto item = index.find(key);

			if (item != index.end())
			{
				item->second->update_value(value);
			}
		}

		ftxui::Elements all_renderer()
		{
			ftxui::Elements out;
			out.reserve(fields.size());
			for (auto& elem : fields)
			{
				out.push_back(elem->renderer());
			}

			return out;
		}

		ftxui::Element renderer()
		{

			return vbox(all_renderer());
		}
	};

	constexpr std::unique_ptr<TextField> make_text_field(const KeyValuePair& pair)
	{
		return std::make_unique<TextField>(pair);
	}

	constexpr std::unique_ptr<LinkField> make_link_field(const LinkField::LinkData& data)
	{
		return std::make_unique<LinkField>(data);
	}

	class MainWindow : public Window
	{
		Content info;
		Content authors;
		Content connect_info;
		Content reactor_state_min;

	public:
		MainWindow(const MainWindow&)			 = default;
		MainWindow(MainWindow&&)				 = default;
		MainWindow& operator=(const MainWindow&) = default;
		MainWindow& operator=(MainWindow&&)		 = default;
		~MainWindow() override					 = default;

		MainWindow()
		{
			set_name("main");

			info.add(make_text_field({.key = "Program name", .val = PROJECT_NAME}));
			info.add(make_text_field({.key = "Version", .val = PROJECT_VERSION}));
			info.add(make_text_field({.key = "Compiler", .val = COMPILER_INFO}));
			info.add(make_text_field({.key = "Build date", .val = BUILD_DATE}));
			info.add(make_text_field({.key = "Build time", .val = BUILD_TIME}));

			authors.add(make_text_field({.key = "Authors", .val = "SamirShef, megonilus"}));
			authors.add(make_link_field(
				{.key = "SamirShef", .val = "github", .link = "https://github.com/SamirShef"}));
			authors.add(make_link_field(
				{.key = "megonilus", .val = "github", .link = "https://github.com/megonilus"}));

			connect_info.add(make_text_field({.key = "Status", .val = "Connected"}));
			connect_info.add(make_text_field({.key = "Ip", .val = "127.0.0.1"}));
			connect_info.add(make_text_field({.key = "Port", .val = "12345"}));
			connect_info.add(make_text_field({.key = "Speed", .val = "10mb/s"}));
			connect_info.add(make_text_field({.key = "Type", .val = "TCP/IP"}));

			reactor_state_min.add(make_text_field({.key = "State", .val = "NORMAL"}));
			reactor_state_min.add(make_text_field({.key = "Current temp", .val = "85.9 Celsius"}));
			reactor_state_min.add(make_text_field({.key = "Humidity", .val = "79%"}));
			reactor_state_min.add(make_text_field({.key = "Pressure", .val = "12.9 Pa"}));
		}

		ftxui::Component component() override;
	};

	class ControlWindow : public Window
	{
	public:
		ControlWindow(const ControlWindow&)			   = default;
		ControlWindow(ControlWindow&&)				   = default;
		ControlWindow& operator=(const ControlWindow&) = default;
		ControlWindow& operator=(ControlWindow&&)	   = default;
		~ControlWindow() override					   = default;

		ControlWindow()
		{
			set_name("control");
		}

		ftxui::Component component() override;
	};

	class StatWindow : public Window
	{
	public:
		StatWindow(const StatWindow&)			 = default;
		StatWindow(StatWindow&&)				 = default;
		StatWindow& operator=(const StatWindow&) = default;
		StatWindow& operator=(StatWindow&&)		 = default;
		~StatWindow() override					 = default;
		StatWindow()
		{
			set_name("stats");
		}

		ftxui::Component component() override;
	};

	class Bar
	{

		std::vector<std::string> tab_names;
		int						 tab_selected = 0;

		MainWindow	  main_window;
		ControlWindow control_window;
		StatWindow	  stat_window;

		ftxui::Component main_component;
		ftxui::Component control_component;
		ftxui::Component stat_component;

	public:
		ftxui::Component renderer();

		Bar(const Bar&)			   = default;
		Bar(Bar&&)				   = default;
		Bar& operator=(const Bar&) = default;
		Bar& operator=(Bar&&)	   = default;
		~Bar()					   = default;

		explicit Bar(std::vector<std::string> tab_names) : tab_names(std::move(tab_names)) {}
		explicit Bar()
		{
			tab_names = std::vector<std::string>(
				{main_window.get_name(), control_window.get_name(), stat_window.get_name()});

			main_component	  = main_window.component();
			control_component = control_window.component();
			stat_component	  = stat_window.component();
		};
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
