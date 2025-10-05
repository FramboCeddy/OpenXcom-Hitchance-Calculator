#pragma once
#include "Structs.h"
#include <vector>

namespace OpenXcom
{
int CalcZShiftVanilla(const Position& Pos);
int CalcZShiftUniform(const Position& Pos);
} // namespace OpenXcom

namespace AccuracyTest
{
constexpr bool PosInCircle(const OpenXcom::Position& pos, int diameter);
bool CircleIntersect(const OpenXcom::Position& startPos, const OpenXcom::Position& endPos, int diameter);
bool CylIntersect(const OpenXcom::Position& startPos, const OpenXcom::Position& endPos, int diameter, double halfHeight);
DevVals TestAllShotsVanilla(const DevVals& prevTest, const OpenXcom::Position& shooterPos, int alienDiameter, double alienHalfHeight, int deviation);
double TestAllShotsUniform(const OpenXcom::Position& shooterPos, int alienDiameter, double alienHalfHeight, int deviation);
void CalcOneDistVanilla(const OpenXcom::Position& distance, const OpenXcom::Unit& alien, bool kneeling = false, int accuracy = -1);
void CalcOneDistUniform(const OpenXcom::Position& distance, const OpenXcom::Unit& alien, bool kneeling = false, int accuracy = -1);
std::vector<double> CalcHitChances(const std::vector<double>& devHitChances);
} // namespace AccuracyTest
