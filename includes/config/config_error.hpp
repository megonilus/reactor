#pragma once
#include <stdexcept>

struct ConfigError : std::runtime_error
{
	using std::runtime_error::runtime_error;
};
