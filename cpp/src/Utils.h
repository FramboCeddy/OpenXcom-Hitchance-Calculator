#pragma once
#include "Structs.h"
#include <vector>

namespace Utils
{
void PrintHitChance(const OpenXcom::Position& pos, const std::string& race, int accuracy, double hitChance);
void WriteHitChancesToFile(const OpenXcom::Position& pos, const std::string& race, const std::vector<double>& devHitChances);
} // namespace Utils
