#pragma once

#include "Structs.h"

namespace OpenXcom
{
    int CalcDeviation(int zShift, double accuracy);

    int CalcZShiftVanilla(const Position &Pos);
    Position CalcShotOffsetVanilla(int deviation);
    Position XCOMAccuracyVanilla(const Position &Pos, double accuracy);

    int CalcZShiftUniform(const Position &Pos);
    Position CalcShotOffsetUniform(int deviation);
    Position XCOMAccuracyUniform(const Position &Pos, double accuracy);
    Position CalcShotOffsetUniformFIX(int deviation);
    Position XCOMAccuracyUniformFIX(const Position &Pos, double accuracy);
}
