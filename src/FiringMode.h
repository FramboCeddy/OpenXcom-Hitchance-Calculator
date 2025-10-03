#pragma once
#include "Structs.h"
namespace temp
{
// TODO: make a class for each of the 3 firing modes (vanilla, uniform, and my own angular creation)
} // namespace temp

class FireMode
{
    virtual AccuracyTest::DevVals TestAllShotsVanilla(const AccuracyTest::DevVals& prevTest, const OpenXcom::Position& shooterPos, int alienDiameter,
                                                      double alienHalfHeight, int deviation);
    virtual void CalcOneDistVanilla(const OpenXcom::Position& distance, const OpenXcom::Unit& alien, bool kneeling = false, int accuracy = -1);
};
