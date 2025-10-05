#pragma once
#include "Structs.h"
#include <string>
#include <tuple>

namespace cli
{
using cliArgs = std::tuple<OpenXcom::Position, OpenXcom::Unit, AccuracyTest::FiringMode, bool, int>;

std::string trim(const std::string& str);
void printHelp(const std::string& progName);
void printUnits();
OpenXcom::Unit parseUnit(std::string Str);
OpenXcom::Position parsePosition(const std::string& coordStr);
auto parseAccuracy(const std::string& accStr);
cliArgs parseCLI(int argc, char* argv[]);
} // namespace cli
