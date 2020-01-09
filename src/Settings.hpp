#pragma once

#include <map>
#include <any>
#include <optional>

class Settings
{
private:
	Settings();

	void loadDefaults();

public:
	static Settings &instance() {
		static Settings s;
		return s;
	}

	Settings(Settings const &) = delete;
	void operator=(Settings const &) = delete;

	~Settings();

public:
	bool load(std::string const &filename);
	const std::string &get(std::string const &name) const;
	void set(std::string const &name, std::string const &value);

private:
	std::map<std::string, std::string> _values;
};
