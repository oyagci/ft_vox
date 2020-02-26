#pragma once

#include <sstream>
#include <unordered_map>
#include "lazy.hpp"

#define PROFILER_START() do { \
		Profiler::Instance().Start(std::string(__PRETTY_FUNCTION__)); \
	} while (false);

#define PROFILER_STOP() do { \
		Profiler::Instance().Stop(std::string(__PRETTY_FUNCTION__)); \
	} while (false);

class Profiler
{
private:
	struct _ProfilerFuncData {
		double beginTime;
		double endTime;
		std::vector<std::string> logs;

		_ProfilerFuncData() {}
		_ProfilerFuncData(double beginTime) : beginTime(beginTime) {}
	};

	std::unordered_map<std::string, _ProfilerFuncData> Data;

	Profiler() {
	}

public:
	Profiler(const Profiler &) = delete;
	void operator=(const Profiler &) = delete;

	static Profiler &Instance() {
		static Profiler profiler;
		return profiler;
	}

	void Start(std::string const &funcName)
	{
		if (Data.find(funcName) != Data.end()) { return ; }

		_ProfilerFuncData data(std::floor(glfwGetTime() * 1000));
		Data[funcName] = data;
	}

	void Stop(std::string const &funcName)
	{
		if (Data.find(funcName) == Data.end()) { return ; }

		_ProfilerFuncData &data = Data[funcName];

		data.endTime = std::floor(glfwGetTime() * 1000);

		std::stringstream funcLog;
		funcLog << data.beginTime << ":" << data.endTime << ":" << funcName << ":" << data.endTime - data.beginTime << " ms";

		data.logs.push_back(funcLog.str());

		std::cerr << funcLog.str() << std::endl;

		Data.erase(funcName);
	}
};
