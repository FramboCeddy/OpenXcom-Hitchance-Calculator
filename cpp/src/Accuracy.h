#pragma once
#include "Structs.h"
#include <vector>

namespace OpenXcom
{
int CalcZShiftVanilla(const Position& pos);
int CalcZShiftUniform(const Position& pos);
int CalcDeviation(int zShift, int deviation);
} // namespace OpenXcom

namespace AccuracyTest
{
constexpr bool PosInCircle(const OpenXcom::Position& pos, int diameter);
bool CircleIntersect(const OpenXcom::Position& startPos, const OpenXcom::Position& endPos, int diameter);
bool CylIntersect(const OpenXcom::Position& startPos, const OpenXcom::Position& endPos, int diameter, double halfHeight);
DevVals TestAllShotsVanilla(const DevVals& prevTest, const OpenXcom::Position& pos, int diameter, double halfHeight, int deviation);
double TestAllShotsUniform(const OpenXcom::Position& pos, int diameter, double halfHeight, int deviation);
void CalcOneDistVanilla(const OpenXcom::Position& pos, const OpenXcom::Unit& unit, bool kneeling = false, int accuracy = -1);
void CalcOneDistUniform(const OpenXcom::Position& pos, const OpenXcom::Unit& unit, bool kneeling = false, int accuracy = -1);
std::vector<double> CalcHitChances(const std::vector<double>& devHitChances);
} // namespace AccuracyTest
