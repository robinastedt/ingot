#pragma once

#include <string>
#include <stdexcept>

#define internal_error(msg) std::runtime_error(std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": internal error: " + (msg))