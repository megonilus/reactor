#pragma once
#include "config.hpp"

#include "config_error.hpp"

#include <filesystem>
#include <format>
#include <string>
#include <toml++/toml.hpp>

namespace cfg
{

	template <typename T>
	T get_required(const toml::table& tbl, std::string_view key, std::string_view section)
	{
		const auto* node = tbl.get(key);
		if (!node)
		{
			throw ConfigError(std::format("[{}] missing required field '{}'", section, key));
		}

		if (!node->is_value())
		{
			throw ConfigError(std::format("[{}] '{}' must be a value", section, key));
		}

		auto val = node->value<T>();
		if (!val)
		{
			throw ConfigError(std::format("[{}] '{}' has invalid type", section, key));
		}

		return *val;
	}

	template <typename T>
	std::optional<T> get_optional(const toml::table& tbl, std::string_view key)
	{
		const auto* node = tbl.get(key);
		if (!node || !node->is_value())
		{
			return std::nullopt;
		}
		return node->value<T>();
	}

	static ReactorConfig load_reactor(const toml::table& root)
	{
		const auto& tbl = root["reactor"].as_table();
		if (tbl == nullptr)
		{
			throw ConfigError("[reactor] section missing");
		}

		ReactorConfig rcfg;

		rcfg.surface_area = get_required<double>(*tbl, "surface_area", "reactor");

		auto& wall				  = rcfg.wall;
		wall.thickness			  = get_required<double>(*tbl, "wall_thickness", "reactor");
		wall.thermal_conductivity = get_optional<double>(*tbl, "wall_thermal_conductivity")
										.value_or(WALL_THERMAL_CONDUCTIVITY);

		return rcfg;
	}

	static MassConfig load_mass(const toml::table& root)
	{
		const auto& tbl = root["mass"].as_table();
		if (tbl == nullptr)
		{
			throw ConfigError("[mass] section missing");
		}

		MassConfig mcfg;
		mcfg.input	= get_required<double>(*tbl, "input", "mass");
		mcfg.output = get_required<double>(*tbl, "output", "mass");
		return mcfg;
	}

	static ReactionConfig load_reaction(const toml::table& root)
	{
		const auto& tbl = root["reaction"].as_table();
		if (tbl == nullptr)
		{
			throw ConfigError("[reaction] section missing");
		}

		ReactionConfig rcfg;

		rcfg.needed_temp	 = get_required<double>(*tbl, "needed_temp", "reaction");
		rcfg.needed_humidity = get_required<double>(*tbl, "needed_humidity", "reaction");
		rcfg.needed_pressure = get_required<double>(*tbl, "needed_pressure", "reaction");
		rcfg.volume			 = get_required<double>(*tbl, "volume", "reaction");

		rcfg.pressure	 = get_required<double>(*tbl, "pressure", "reaction");
		rcfg.humidity	 = get_required<double>(*tbl, "humidity", "reaction");
		rcfg.temperature = get_required<double>(*tbl, "temperature", "reaction");

		const auto* energy_tbl = tbl->get("energy")->as_table();
		if (energy_tbl == nullptr)
		{
			throw ConfigError("[reaction.energy] section missing");
		}

		rcfg.energy.consumption =
			get_required<double>(*energy_tbl, "consumption", "reaction.energy");
		rcfg.energy.max_consumption =
			get_required<double>(*energy_tbl, "max_consumption", "reaction.energy");

		rcfg.ambient_temperature =
			get_optional<double>(*tbl, "ambient_temperature").value_or(rcfg.ambient_temperature);
		rcfg.specific_gas_constant = get_optional<double>(*tbl, "specific_gas_constant")
										 .value_or(rcfg.specific_gas_constant);
		rcfg.heat_transfer_coefficient = get_optional<double>(*tbl, "heat_transfer_coefficient")
											 .value_or(rcfg.heat_transfer_coefficient);
		rcfg.cooling_rate = get_optional<double>(*tbl, "cooling_rate").value_or(rcfg.cooling_rate);
		rcfg.heating_rate = get_optional<double>(*tbl, "heating_rate").value_or(rcfg.heating_rate);

		rcfg.heat_capacity		  = get_required<double>(*tbl, "heat_capacity", "reaction");
		rcfg.thermal_conductivity = get_required<double>(*tbl, "thermal_conductivity", "reaction");

		rcfg.min_temp	  = get_required<double>(*tbl, "min_temp", "reaction");
		rcfg.max_temp	  = get_required<double>(*tbl, "max_temp", "reaction");
		rcfg.max_humidity = get_required<double>(*tbl, "max_humidity", "reaction");
		rcfg.max_pressure = get_required<double>(*tbl, "max_pressure", "reaction");

		return rcfg;
	}
	AppConfig load_config(const std::string& path)
	{
		namespace fs = std::filesystem;

		if (!fs::exists(path))
		{
			std::ofstream ofs(path);
			if (!ofs.is_open())
			{
				throw std::runtime_error("Failed to create default config file: " + path);
			}

			ofs << "[reactor]\n";
			ofs << "surface_area = 1.0\n";
			ofs << "wall_thickness = 0.1\n";
			ofs << "wall_thermal_conductivity = 0.005\n\n";

			ofs << "[mass]\n";
			ofs << "input = 1.0\n";
			ofs << "output = 1.0\n\n";

			ofs << "[reaction]\n";
			ofs << "needed_temp = 300.0\n";
			ofs << "needed_humidity = 30.0\n";
			ofs << "needed_pressure = 101325.0\n";
			ofs << "volume = 1.0\n";
			ofs << "heat_capacity = 4180.0\n";
			ofs << "thermal_conductivity = 0.6\n";
			ofs << "min_temp = 273.0\n";
			ofs << "max_temp = 500.0\n";
			ofs << "max_pressure = 1000000.0\n";
			ofs << "max_humidity = 100.0\n";
			ofs << "pressure = 101325.0\n";
			ofs << "humidity = 50.0\n";
			ofs << "temperature = 293.0\n";

			ofs << "\n[reaction.energy]\n";
			ofs << "consumption = 1000.0\n";
			ofs << "max_consumption = 20000.0\n\n";

			ofs << "# Optional values and their default values\n";
			ofs << "# ambient_temperature = 293.0\n";
			ofs << "# specific_gas_constant = 287.0\n";
			ofs << "# heat_transfer_coefficient = 0.05\n";
			ofs << "# cooling_rate = 0.0\n";
			ofs << "# heating_rate = 15000.0\n";
		}

		toml::table root;
		try
		{
			root = toml::parse_file(path);
		}
		catch (const toml::parse_error& e)
		{
			throw ConfigError(std::format("TOML parse error: {}", e.description()));
		}

		AppConfig cfg;
		cfg.reactor	 = load_reactor(root);
		cfg.mass	 = load_mass(root);
		cfg.reaction = load_reaction(root);

		return cfg;
	}
	std::string config_path()
	{
		namespace fs = std::filesystem;

		std::string base_dir;

#if defined(REACTOR_LINUX) || defined(REACTOR_UNIX) || defined(REACTOR_POSIX) ||                   \
	defined(REACTOR_CYGWIN)
		// Linux / UNIX
		const char* home = std::getenv("HOME");
		if (!home) throw std::runtime_error("Environment variable HOME not set");

		base_dir = std::string(home) + "/.config/reactor";

#elif defined(REACTOR_APPLE)
		const char* home = std::getenv("HOME");
		if (!home) throw std::runtime_error("Environment variable HOME not set");

		base_dir = std::string(home) + "/Library/Application Support/reactor";

#elif defined(REACTOR_WINDOWS)
		const char* appdata = std::getenv("APPDATA");
		if (!appdata) throw std::runtime_error("Environment variable APPDATA not set");

		base_dir = std::string(appdata) + "\\reactor";

#else
		// Fallback — POSIX style
		const char* home = std::getenv("HOME");
		if (home == nullptr)
		{
			throw std::runtime_error("Environment variable HOME not set");
		}

		base_dir = std::string(home) + "/.config/reactor";

#endif

		fs::path file_path = fs::path(base_dir) / "config.toml";

		try
		{
			fs::create_directories(fs::path(base_dir));
		}
		catch (const std::exception& e)
		{
			throw std::runtime_error(std::string("Failed to create config directory: ") + e.what());
		}

		return file_path.string();
	}
} // namespace cfg
