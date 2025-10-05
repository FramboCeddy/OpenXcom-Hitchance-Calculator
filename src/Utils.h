#pragma once
#include "Structs.h"
#include <vector>

namespace Utils
{
void printHitChance(const OpenXcom::Position& distance, const std::string& alienRace, int accuracy, double hitChance);
void writeHitChancesToFile(const OpenXcom::Position& distance, const std::string& alienRace, const std::vector<double>& devHitChances);
} // namespace Utils
