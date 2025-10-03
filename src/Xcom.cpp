#include "Xcom.h"
#include "RNG.h"
#include "Structs.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>

namespace OpenXcom
{
int CalcDeviation(int zShift, double accuracy)
{
    int deviation = RNG::generate(0, 100) - (accuracy * 100);
    // This makes accuracy behave similar to a binary hit/miss system.
    if (deviation >= 0)
        deviation += 50;
    else
        deviation += 10;
    // Max deviation is 150 => 3/4 * zshift
    deviation = std::max(1, zShift * deviation / 200);
    return deviation;
}

// Calculates the zShift based on the position of the shooter for vanilla spread.
int CalcZShiftVanilla(const Position& Pos)
{
    int xDist = std::abs(Pos.x);
    int yDist = std::abs(Pos.y);
    int zDist = std::abs(Pos.z);
    int xyShift, zShift;

    if (xDist / 2 <= yDist)          // yes, we need to add some x/y non-uniformity
        xyShift = xDist / 4 + yDist; // and don't ask why, please. it's The Commandment
    else
        xyShift = (xDist + yDist) / 2; // that's uniform part of spreading

    if (xyShift <= zDist) // slight z deviation
        zShift = xyShift / 2 + zDist;
    else
        zShift = xyShift + zDist / 2;

    return zShift;
}

// Calculates the shot offset based on the deviation for vanilla spread.
Position CalcShotOffsetVanilla(int deviation)
{
    // adding to target coords (0, 0, 0) is equal to the values themselves
    int offsetX = RNG::generate(0, deviation) - deviation / 2;
    int offsetY = RNG::generate(0, deviation) - deviation / 2;
    int offsetZ = RNG::generate(0, deviation / 2) / 2 - deviation / 8;
    return Position(offsetX, offsetY, offsetZ);
}

/*	This is the default Vanilla shooting spread implementation.
    We only use 1 position as we assume the target is located at the origin.
    We skip any accuracy modifiers (like noLOS, UFOExtender, kneeling, etc.),
    as accuracy will be the final accuracy in-game that you see on your crosshair.	*/
Position XCOMAccuracyVanilla(const Position& Pos, double accuracy)
{
    int zShift = CalcZShiftVanilla(Pos);
    int deviation = CalcDeviation(zShift, accuracy);
    return CalcShotOffsetVanilla(deviation);
}

// Calculates the zShift based on the position of the shooter for uniform spread.
int CalcZShiftUniform(const Position& Pos)
{
    int xDist = std::abs(Pos.x);
    int yDist = std::abs(Pos.y);
    int zDist = std::abs(Pos.z);
    int xyShift, zShift;

    if (xDist <= yDist)
        xyShift = xDist / 4 + yDist;
    else
        xyShift = xDist + yDist / 4;

    xyShift *= 0.839; // Constant to match average xyShift to vanilla

    if (xyShift <= zDist) // slight z deviation
        zShift = xyShift / 2 + zDist;
    else
        zShift = xyShift + zDist / 2;

    return zShift;
}

// Calculates the shot offset based on the position of the shooter and the deviation for uniform spread.
Position CalcShotOffsetUniform(int deviation)
{
    // First, new target point is rolled as usual. Then, if it lies outside of outer circle (in square's corner)
    // it's rerolled inside inner circle
    const double SECONDARY_SPREAD_COEFF = 0.85; // Inner spread circle diameter compared to outer

    bool resultShifted = false;
    int dX, dY;

    for (int i = 0; i < 10; ++i) // Break from this cycle when proper target is found
    {
        dX = RNG::generate(0, deviation) - deviation / 2;
        dY = RNG::generate(0, deviation) - deviation / 2;

        int radiusSq = dX * dX + dY * dY;
        int deviateRadius = deviation / 2;
        int deviateRadiusSq = deviateRadius * deviateRadius;

        if (radiusSq <= deviateRadiusSq)
        {
            break; // If we inside of the spread circle - we're done!
        }
        if (!resultShifted)
        {
            resultShifted = true;
            deviation *= SECONDARY_SPREAD_COEFF; // Change spread radius for second+ attempts
        }
    }
    // adding to target coords (0, 0, 0) are the values themselves
    int offsetX = dX;
    int offsetY = dY;
    int offsetZ = RNG::generate(0, deviation / 2) / 2 - deviation / 8;
    return Position(offsetX, offsetY, offsetZ);
}

/*	The new and improved calculations for accuracy when you enable uniform spread.
We only use 1 position as we assume the target is located at the origin.
We skip any accuracy modifiers (like noLOS, UFOExtender, kneeling, etc.),
as accuracy will be the final accuracy in-game that you see on your crosshair.	*/
Position XCOMAccuracyUniform(const Position& Pos, double accuracy)
{
    int zShift = CalcZShiftUniform(Pos);
    int deviation = CalcDeviation(zShift, accuracy);
    return CalcShotOffsetUniform(deviation);
}

Position CalcShotOffsetUniformFIX(int deviation)
{
    // First, new target point is rolled as usual. Then, if it lies outside of outer circle (in square's corner)
    // it's rerolled

    int deviateRadiusSq = deviation * deviation / 4;
    int radiusSq;
    int dX, dY;
    do
    {
        dX = RNG::generate(0, deviation) - deviation / 2;
        dY = RNG::generate(0, deviation) - deviation / 2;
        radiusSq = dX * dX + dY * dY;
    } while (radiusSq > deviateRadiusSq);

    // adding to target coords (0, 0, 0) are the values themselves
    int offsetX = dX;
    int offsetY = dY;
    int offsetZ = RNG::generate(0, deviation / 2) / 2 - deviation / 8;
    return Position(offsetX, offsetY, offsetZ);
}

Position XCOMAccuracyUniformFIX(const Position& Pos, double accuracy)
{
    int zShift = CalcZShiftUniform(Pos);
    int deviation = CalcDeviation(zShift, accuracy);
    return CalcShotOffsetUniformFIX(deviation);
}
} // namespace OpenXcom
