#pragma once

#include <cmath>
#include <common.hpp>
#include <cstdint>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/flexbox_config.hpp>
#include <ftxui/dom/node.hpp>
#include <ftxui/screen/color.hpp>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <utility>
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

using namespace ftxui;

namespace tui
{
	using FieldValue = std::variant<std::string, double>;
	using Key		 = std::string;
	using ColorTuple = std::tuple<uint8_t, uint8_t, uint8_t>;

	class Window
	{
		static constexpr uint8_t DEFAULT_R = 60;
		static constexpr uint8_t DEFAULT_G = 179;
		static constexpr uint8_t DEFAULT_B = 113;

		std::string name		= "window";
		ColorTuple	title_color = std::make_tuple(DEFAULT_R, DEFAULT_G, DEFAULT_B);

	public:
		Window(const Window&)			 = default;
		Window(Window&&)				 = default;
		Window& operator=(const Window&) = default;
		Window& operator=(Window&&)		 = default;
		virtual ~Window()				 = default;
		Window()						 = default;

		virtual Component component()
		{
			return Renderer(
				[this]
				{
					return window(ftxui::text(name) |
									  color(Color::RGB(get<0>(title_color), get<1>(title_color),
													   get<2>(title_color))),
								  text("test" + name));
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

		void set_color(const ColorTuple& new_color)
		{
			title_color = new_color;
		}
	};

	class BaseField
	{
		Key		key;
		Element cached_element = ftxui::text("");
		bool	dirty		   = true;

		std::function<FieldValue()> provider;

	protected:
		virtual void update_value_impl(const FieldValue& value)
		{
			if (std::holds_alternative<std::string>(value))
			{
				cached_element = text(std::get<std::string>(value));
			}
		};

		virtual Element element_impl()
		{
			return text("");
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

		Element element()
		{
			if (dirty)
			{
				auto element   = element_impl();
				dirty		   = false;
				cached_element = element;
			}

			return cached_element;
		}

		[[nodiscard]] Key get_key() const noexcept
		{
			return key;
		}

		void set_provider(std::function<FieldValue()> provider)
		{
			this->provider = std::move(provider);
		}

		virtual void rerender()
		{
			if (provider)
			{
				update_value(provider());
			}
			else
			{
				dirty = true;
			}
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

		Element element_impl() override
		{
			constexpr uint8_t DEFAULT_LINK_G = 102;
			constexpr uint8_t DEFAULT_LINK_B = 204;

			return hbox({text(get_key()), ftxui::filler(),
						 hyperlink(link, ftxui::text(value)) |
							 color(ftxui::Color::RGB(0, DEFAULT_LINK_G, DEFAULT_LINK_B))});
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

		Element element_impl() override
		{
			return hbox({ftxui::text(get_key()), ftxui::filler(), ftxui::text(value)});
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

		template <class Obj, class MemFn> void add_auto(const std::string& key, Obj* obj, MemFn fun)
		{
			auto ptr = std::make_unique<TextField>(KeyValuePair{.key = key, .val = ""});

			ptr->set_provider(
				[obj, fun]
				{
					std::scoped_lock lock(obj->mutex);
					return FieldValue((obj->*fun)());
				});

			index[key] = ptr.get();
			fields.push_back(std::move(ptr));
		}

		void update_by_key(const Key& key, const FieldValue& value)
		{
			auto item = index.find(key);

			if (item != index.end())
			{
				item->second->update_value(value);
			}
		}

		Elements elements() const
		{
			Elements out;
			out.reserve(fields.size());
			for (const auto& elem : fields)
			{
				out.push_back(elem->element());
			}

			return out;
		}

		Element element() const
		{
			return vbox(elements());
		}

		void rerender_all()
		{
			for (auto& field : fields)
			{
				field->rerender();
			}
		}

		void
		add_auto_many(State*																   obj,
					  std::initializer_list<std::pair<std::string, double (State::*)() const>> list)
		{
			for (const auto& item : list)
			{
				add_auto(item.first, obj, item.second);
			}
		}
	};

	class ContentCell
	{
		std::string name;
		Content		content;

	public:
		ContentCell(const ContentCell&)			   = default;
		ContentCell(ContentCell&&)				   = default;
		ContentCell& operator=(const ContentCell&) = default;
		ContentCell& operator=(ContentCell&&)	   = default;
		~ContentCell()							   = default;
		explicit ContentCell(std::string cell_name) : name(std::move(cell_name)) {};

		[[nodiscard]] Content& get_content()
		{
			return content;
		}

		const Content& get_const_content() const
		{
			return content;
		}

		Element element() const
		{
			return window(text(name), content.element());
		}
	};

	class GraphField : public BaseField
	{
		bool									  fake = true;
		std::string								  name = "graph";
		std::function<std::vector<int>(int, int)> provider;
		ftxui::Color							  style_color = Color::BlueLight;
		std::function<std::vector<int>(int, int)> fake_provider;
		std::chrono::steady_clock::time_point	  start_time;

	protected:
		ftxui::Element fake_element()
		{
			using namespace ftxui;
			auto graph_el = graph(fake_provider) | color(style_color) | flex;
			return vbox({
					   text(name) | bold | hcenter,
					   separator(),
					   graph_el,
				   }) |
				   border;
		}

		ftxui::Element element_impl() override
		{
			using namespace ftxui;
			if (!provider)
			{
				return fake_element();
			}

			return vbox({
					   text(name) | bold | hcenter,
					   separator(),
					   graph(provider) | color(style_color) | flex,
				   }) |
				   border;
		}

	public:
		struct GraphData
		{
			bool		is_fake = true;
			std::string name;
		};

		GraphField(const GraphField&)			 = default;
		GraphField(GraphField&&)				 = delete;
		GraphField& operator=(const GraphField&) = default;
		GraphField& operator=(GraphField&&)		 = delete;
		~GraphField() override					 = default;
		GraphField()							 = delete;

		explicit GraphField(GraphData data)
			: BaseField(data.name),
			  fake(data.is_fake),
			  name(std::move(data.name)),
			  start_time(std::chrono::steady_clock::now())
		{
			constexpr double SPATIAL_FREQ_A = 0.10;
			constexpr double SPATIAL_FREQ_B = 0.15;
			constexpr double SPATIAL_FREQ_C = 0.03;
			constexpr double AMPLITUDE_A	= 0.10;
			constexpr double AMPLITUDE_B	= 0.20;
			constexpr double AMPLITUDE_C	= 0.10;
			constexpr double BASE_OFFSET	= 0.5;
			constexpr double PHASE_OFFSET	= 10.0;
			constexpr double TEMPORAL_SPEED = 1.0;

			fake_provider = [start = start_time](int width, int height) -> std::vector<int>
			{
				if (width <= 0 || height <= 0)
				{
					return {};
				}

				std::vector<int> output;
				output.resize(static_cast<size_t>(width));

				auto   now_tp = std::chrono::steady_clock::now();
				double time =
					std::chrono::duration<double>(now_tp - start).count() * TEMPORAL_SPEED;

				for (int i = 0; i < width; ++i)
				{
					double splate_1 = AMPLITUDE_A * std::sin((i * SPATIAL_FREQ_A) + time);
					double splate_2 =
						AMPLITUDE_B * std::sin((i * SPATIAL_FREQ_B) + time + PHASE_OFFSET);
					double splate_3 = AMPLITUDE_C * std::sin((i * SPATIAL_FREQ_C) + time);
					double value	= splate_1 + splate_2 + splate_3 + BASE_OFFSET;
					output[static_cast<size_t>(i)] =
						static_cast<int>(value * static_cast<double>(height));
				}

				return output;
			};
		}

		template <typename F> void set_provider(F&& some)
		{
			provider = std::function<std::vector<int>(int, int)>(std::forward<F>(some));
			fake	 = false;
		}

		void clear_provider()
		{
			provider = nullptr;
			fake	 = true;
		}

		void set_name(std::string n)
		{
			name = std::move(n);
		}
		void set_color(ftxui::Color new_color)
		{
			style_color = new_color;
		}

		ftxui::Element element()
		{
			if (fake)
			{
				return fake_element();
			}
			return element_impl();
		}
	};
	inline std::unique_ptr<TextField> make_text_field(const KeyValuePair& pair)
	{
		return std::make_unique<TextField>(pair);
	}

	inline std::unique_ptr<LinkField> make_link_field(const LinkField::LinkData& data)
	{
		return std::make_unique<LinkField>(data);
	}

	inline std::unique_ptr<GraphField> make_graph_field(const GraphField::GraphData& data)
	{
		return std::make_unique<GraphField>(data);
	}

	inline std::unique_ptr<TextField> make_text_field_provider(const Key&				   key,
															   std::function<FieldValue()> provider)
	{
		auto ptr = std::make_unique<TextField>(KeyValuePair{.key = key, .val = ""});
		ptr->set_provider(std::move(provider));
		return ptr;
	}

	class MainWindow : public Window
	{
		State*		state;
		ContentCell info;
		ContentCell reactor_state_min;

	public:
		MainWindow(const MainWindow&)			 = default;
		MainWindow(MainWindow&&)				 = default;
		MainWindow& operator=(const MainWindow&) = default;
		MainWindow& operator=(MainWindow&&)		 = default;
		~MainWindow() override					 = default;

		explicit MainWindow(State* state) : state(state), info("Info"), reactor_state_min("Reactor")
		{
			set_name("Main Control");

			info.get_content().add(make_text_field({.key = "Program name", .val = PROJECT_NAME}));
			info.get_content().add(make_text_field({.key = "Version", .val = PROJECT_VERSION}));
			info.get_content().add(make_text_field({.key = "Compiler", .val = COMPILER_INFO}));
			info.get_content().add(make_text_field({.key = "Build date", .val = BUILD_DATE}));
			info.get_content().add(make_text_field({.key = "Build time", .val = BUILD_TIME}));
			info.get_content().add(
				make_text_field({.key = "Authors", .val = "SamirShef, megonilus"}));
			info.get_content().add(make_link_field(
				{.key = "SamirShef", .val = "github", .link = "https://github.com/SamirShef"}));
			info.get_content().add(make_link_field(
				{.key = "megonilus", .val = "github", .link = "https://github.com/megonilus"}));
			info.get_content().add(
				make_link_field({.key  = "Source code",
								 .val  = "github",
								 .link = "https://github.com/megonilus/reactor"}));
		}

		Component component() override;
	};
	class StatWindow : public Window
	{
		State*		state;
		ContentCell indicators;

	public:
		StatWindow(const StatWindow&)			 = default;
		StatWindow(StatWindow&&)				 = default;
		StatWindow& operator=(const StatWindow&) = default;
		StatWindow& operator=(StatWindow&&)		 = default;
		~StatWindow() override					 = default;

		explicit StatWindow(State* state) : state(state), indicators("Indicators")
		{
			set_name("stats");

			indicators.get_content().add_auto_many(
				state, {{"Temp", &State::get_temperature},
						{"Needed temp", &State::get_needed_temperature},
						{"Pressure", &State::get_pressure},
						{"Needed pressure", &State::get_needed_pressure},
						{"Humidity", &State::get_humidity},
						{"Needed humidity", &State::get_needed_humidity},
						{"Mass", &State::get_mass},
						{"Volume", &State::get_volume},
						{"Specific gas const", &State::get_specific_gas_constant},
						{"Heat capacity", &State::get_heat_capacity},
						{"Thermal conductivity", &State::get_thermal_conductivity},
						{"Surface area", &State::get_surface_area},
						{"Wall thickness", &State::get_wall_thickness},
						{"Wall thermal cond.", &State::get_wall_thermal_conductivity},
						{"Ambient temp", &State::get_ambient_temperature},
						{"Heat transfer coeff.", &State::get_heat_transfer_coefficient},
						{"Reaction heat rate", &State::get_reaction_heat_rate},
						{"Cooling rate", &State::get_cooling_rate},
						{"Heating rate", &State::get_heating_rate}});
		}

		Component component() override;
	};

	class Bar
	{
		State* state;

		std::vector<std::string> tab_names;
		int						 tab_selected = 0;

		MainWindow main_window;
		StatWindow stat_window;

		Component main_component;
		Component stat_component;

	public:
		Component component();

		Bar(const Bar&)			   = default;
		Bar(Bar&&)				   = default;
		Bar& operator=(const Bar&) = default;
		Bar& operator=(Bar&&)	   = default;
		~Bar()					   = default;

		explicit Bar(State* state) : state(state), main_window(state), stat_window(state)
		{
			tab_names = std::vector<std::string>({main_window.get_name(), stat_window.get_name()});

			main_component = main_window.component();
			stat_component = stat_window.component();
		};
	};

	class Instance
	{
		std::string name = "Main instance";
		State*		state;

	public:
		explicit Instance(State* state) : state(state) {}

		Instance(const Instance&)			 = delete;
		Instance(Instance&&)				 = default;
		Instance& operator=(const Instance&) = delete;
		Instance& operator=(Instance&&)		 = default;
		~Instance()							 = default;

		// runs and displays
		void display();
	};
} // namespace tui
