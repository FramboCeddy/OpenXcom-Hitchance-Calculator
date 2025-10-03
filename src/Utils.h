#pragma once
#include "Constants.h"
#include "Structs.h"
#include <vector>

namespace Utils
{
void AddDistances(std::vector<OpenXcom::Position>& distanceVector, Constants::Direction direction, int maxDistance, int stepSize);
void printHitChance(const OpenXcom::Position& distance, const std::string& alienRace, int accuracy, double hitChance);
void writeHitChancesToFile(const OpenXcom::Position& distance, const std::string& alienRace, const std::vector<double>& devHitChances);
} // namespace Utils
