#include "Settings.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <chrono>
#include <string>

Settings::Settings()
{
	loadDefaults();
}

Settings::~Settings()
{
}

void Settings::loadDefaults()
{
	_values["renderDistance"] = "14";
	{
		auto now = std::chrono::system_clock::now();
		auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
		auto epoch = now_ms.time_since_epoch();
		auto value = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);
		long duration = value.count();

		_values["seed"] = std::to_string(duration);
	}
}

bool Settings::load(std::string const &filename)
{
	std::ifstream file(filename, std::ios_base::in);

	if (!file.is_open()) {
		file.open(filename, std::ios_base::out | std::ios_base::in);
		return true;
	}

	std::string key;
	std::string value;
	while (std::getline(std::getline(file, key, '=') >> std::ws, value)) {
		_values[key] = value;
	}

	return true;
}

const std::string &Settings::get(std::string const &name) const
{
	return _values.at(name);
}

void Settings::set(std::string const &name, std::string const &value)
{
	_values[name] = value;
}
