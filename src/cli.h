#pragma once
#include "Structs.h"
#include <string>
#include <tuple>

namespace cli
{
std::string trim(const std::string& str);
void printHelp(const std::string& progName);
void printUnits();
OpenXcom::Unit parseUnit(const std::string& Str);
OpenXcom::Position parsePosition(const std::string& coordStr);
auto parseAccuracy(const std::string& accStr);
std::tuple<OpenXcom::Position, OpenXcom::Unit, AccuracyTest::FiringMode, bool, int> parseCLI(int argc, char* argv[]);
} // namespace cli
