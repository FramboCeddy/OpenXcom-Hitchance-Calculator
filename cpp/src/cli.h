#pragma once
#include "Structs.h"
#include <string>
#include <tuple>

namespace cli
{
using cliArgs = std::tuple<OpenXcom::Position, OpenXcom::Unit, AccuracyTest::FiringMode, bool, int>;

std::string TrimWhitespace(const std::string& str);
void PrintHelp(const std::string& progName);
void ListUnits();
OpenXcom::Unit ParseUnit(std::string Str);
OpenXcom::Position ParsePosition(const std::string& coordStr);
int ParseAccuracy(const std::string& accStr);
cliArgs ParseCLI(int argc, char* argv[]);
} // namespace cli
