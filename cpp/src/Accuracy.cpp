#include "Accuracy.h"
#include "Structs.h"
#include "Utils.h"
#include <cmath>
#include <iostream>
#include <numeric>

namespace OpenXcom
{
// Calculates the zShift based on the position of the shooter for vanilla spread.
int CalcZShiftVanilla(const Position& pos)
{
    int xDist = std::abs(pos.x);
    int yDist = std::abs(pos.y);
    int zDist = std::abs(pos.z);
    int xyShift, zShift;

    if (xDist / 2 <= yDist)
    {
        xyShift = xDist / 4 + yDist;
    }
    else
    {
        xyShift = (xDist + yDist) / 2;
    }

    if (xyShift <= zDist)
    {
        zShift = xyShift / 2 + zDist;
    }
    else
    {
        zShift = xyShift + zDist / 2;
    }
    return zShift;
}
// Calculates the zShift based on the position of the shooter for uniform spread.
int CalcZShiftUniform(const Position& pos)
{
    int xDist = std::abs(pos.x);
    int yDist = std::abs(pos.y);
    int zDist = std::abs(pos.z);
    int xyShift, zShift;

    if (xDist <= yDist)
    {
        xyShift = xDist / 4 + yDist;
    }
    else
    {
        xyShift = xDist + yDist / 4;
    }

    xyShift *= 0.839;

    if (xyShift <= zDist)
    {
        zShift = xyShift / 2 + zDist;
    }
    else
    {
        zShift = xyShift + zDist / 2;
    }
    return zShift;
}
/* We don't use RNG and instead iterate over all possible deviations
 * by starting deviation off at accuracy and incrementing up to and including acc + 100
 * so that 100 - dev is equivalent to RNG::generate(0, 100) - acc. */
int CalcDeviation(int zShift, int deviation)
{
    return (deviation <= 100)
               ? std::max(1, zShift * (150 - deviation) / 200)
               : std::max(1, zShift * (110 - deviation) / 200);
}
} // namespace OpenXcom

namespace AccuracyTest
{
constexpr bool PosInCircle(const OpenXcom::Position& pos, int diameter)
{
    // distance of point (x, y) to circle centered on origin < circle radius
    return 4 * (pos.x * pos.x) + 4 * (pos.y * pos.y) < diameter * diameter;
}
bool CircleIntersect(const OpenXcom::Position& startPos, const OpenXcom::Position& endPos, int diameter)
{
    bool isInCircle{PosInCircle(endPos, diameter)};
    // If our shot position is within the circle, we will intersect
    if (isInCircle)
    {
        return true;
    }
    // Ax + By + C = 0
    // NOTE: Could also use doubles, Don't know which is better performance
    long long A{startPos.y - endPos.y};
    long long B{endPos.x - startPos.x};
    long long C{startPos.x * endPos.y - endPos.x * startPos.y};

    // shot parrallel to z-axis => shot will miss if it's outside circle
    if (A == 0 && B == 0)
    {
        // Note: because we are returning early when isIncircle is true, this is essentially return false
        return isInCircle;
    }
    // distance of point (0, 0) to line < circle radius
    double lineDistSq{static_cast<double>(C * C) / static_cast<double>(A * A + B * B)};
    return 4 * lineDistSq < diameter * diameter;
}
bool CylIntersect(const OpenXcom::Position& startPos, const OpenXcom::Position& endPos, int diameter, double halfHeight)
{
    // If our shot position is within the hitbox of the alien,
    // it will always intersect, no matter the angle or distance.
    bool isInCircle{PosInCircle(endPos, diameter)};
    bool isWithinHeight{std::abs(endPos.z) < halfHeight};
    if (isInCircle && isWithinHeight)
    {
        return true;
    }

    int dx{endPos.x - startPos.x};
    int dy{endPos.y - startPos.y};
    int dz{endPos.z - startPos.z};

    // At^2 + Bt + C = 0
    // Quadratic coefficients for intersection with circular base
    // using 4*C from the Discriminant so that all values are ints.
    // x^2 + y^2 - r^2 => 4(x^2 + y^2) - d^2
    long long A{dx * dx + dy * dy};
    long long B{2 * (startPos.x * dx + startPos.y * dy)};
    long long Cmult4{4 * (startPos.x * startPos.x + startPos.y * startPos.y) - (diameter * diameter)};

    // Shot parallel to Z-axis => Intersection occurs if shot is inside circle
    if (A == 0)
    {
        return isInCircle;
    }

    // shot has some XY direction
    // see if our shot in XY plane intersects the circle
    long long D{B * B - A * Cmult4};
    if (D <= 0)
    {
        // Miss circle in XY-plane => Miss cylinder
        return false;
    }
    // get the t-value along shot's direction for the intersection points
    const double sqrt_D{std::sqrt(D)};
    const double t1{(-B - sqrt_D) / (2.0 * A)};
    const double t2{(-B + sqrt_D) / (2.0 * A)};
    // get the height for the intersection points
    const double z1{startPos.z + t1 * dz};
    const double z2{startPos.z + t2 * dz};
    // we miss the cylinder if and only if the intersection points are both above or both below the cylinder
    if ((halfHeight < z1 && halfHeight < z2) || (z1 < -halfHeight && z2 < -halfHeight))
    {
        return false;
    }
    // Other options include 1 point above and 1 point below (= intersect the top and bottom)
    // and 1 or both points within the height bounds (= that point intersects the side)
    return true;
}

/*  Check All possible dx, dy, dz, rather than randomly selecting.
    Also takes in results from a previous test to skip an inner box and only test a hollow box.
    The benefit is that the hit chance is calculated and therefore has no variance. */
DevVals TestAllShotsVanilla(const DevVals& prevTest, const OpenXcom::Position& pos, int diameter, double halfHeight, int deviation)
{
    if (deviation == prevTest.deviation)
    {
        // the boxes will be exactly the same so return previous results
        return prevTest;
    }
    // figure out if previous deviation is bigger or smaller
    const bool devIncreased{deviation > prevTest.deviation};
    const int bigBoxDev{devIncreased ? deviation : prevTest.deviation};
    const int smallBoxDev{devIncreased ? prevTest.deviation : deviation};
    // Get ranges of both deviation boxes
    const int bigXYLower{-bigBoxDev / 2};
    const int bigXYUpper{bigBoxDev - bigBoxDev / 2};
    const int bigZLower{-bigBoxDev / 8};
    const int bigZUpper{(bigBoxDev / 2) / 2 - bigBoxDev / 8};
    const int smallXYLower{-smallBoxDev / 2};
    const int smallXYUpper{smallBoxDev - smallBoxDev / 2};
    const int smallZLower{-smallBoxDev / 8};
    const int smallZUpper{(smallBoxDev / 2) / 2 - smallBoxDev / 8};
    int countHits{0};
    OpenXcom::Position shotPos{0, 0, 0}; // will be filled in later
    // Loop over big box but skip any x,y,z already in small box (hollow box
    // skip)
    for (int x{bigXYLower}; x <= bigXYUpper; ++x)
    {
        shotPos.x = x;
        const bool InsideSmallBoxX{smallXYLower <= x && x <= smallXYUpper};
        for (int y{bigXYLower}; y <= bigXYUpper; ++y)
        {
            shotPos.y = y;
            const bool InsideSmallBoxY{smallXYLower <= y && y <= smallXYUpper};
            // Missing the circle in XY plane always means miss.
            if (!CircleIntersect(pos, shotPos, diameter))
            {
                continue;
            }
            for (int z{bigZLower}; z <= bigZUpper; ++z)
            {
                const bool InsideSmallBoxZ{smallZLower <= z && z <= smallZUpper};
                //  this position falls within our small box so is already tested
                if (InsideSmallBoxX && InsideSmallBoxY && InsideSmallBoxZ)
                {
                    continue;
                }
                shotPos.z = z;
                if (CylIntersect(pos, shotPos, diameter, halfHeight))
                {
                    ++countHits;
                }
            }
        }
    }
    if (devIncreased)
    {
        // the new deviation is the bigger box
        const int numShots{(bigBoxDev + 1) * (bigBoxDev + 1) * (bigBoxDev / 4 + 1)};
        return DevVals{deviation, prevTest.numHits + countHits, numShots};
    }
    else
    {
        // the new deviation is the smaller box
        const int numShots{(smallBoxDev + 1) * (smallBoxDev + 1) * (smallBoxDev / 4 + 1)};
        return DevVals{deviation, prevTest.numHits - countHits, numShots};
    }
}

double TestAllShotsUniform(const OpenXcom::Position& pos, int diameter, double halfHeight, int deviation)
{
    // This is an intentional narrowing conversion as OXCE source code uses
    // deviation *= 0.85 (and deviation holds an int)
    const int secDeviation{static_cast<int>(deviation * 0.85)};
    const int devRadiusSq{(deviation / 2) * (deviation / 2)};
    const int secoDevRadiusSq{(secDeviation / 2) * (secDeviation / 2)};
    const int bigXYLower{-deviation / 2};
    const int bigXYUpper{deviation - deviation / 2};
    const int bigZLower{-deviation / 8};
    const int bigZUpper{(deviation / 2) / 2 - deviation / 8};
    // if result gets shifted it lowers deviation in place and afterwards
    // calculates a dz with this lowered deviation
    const int smallZLower{-secDeviation / 8};
    const int smallZUpper{(secDeviation / 2) / 2 - secDeviation / 8};
    int priHits{0};
    int priShots{0};
    int secHits{0};
    int secShots{0};
    int posOutCircle{0}; // dx and dy fall outside circle
    int posInCircle{0};  // dx and dy fall inside circle so get used

    OpenXcom::Position shotPos{0, 0, 0}; // will be filled in later
    for (int x{bigXYLower}; x <= bigXYUpper; ++x)
    {
        shotPos.x = x;
        for (int y{bigXYLower}; y <= bigXYUpper; ++y)
        {
            shotPos.y = y;

            const int radiusSq{x * x + y * y};
            // radiusSq <= deviateRadiusSq for first roll
            const bool isInBigCircle{radiusSq <= devRadiusSq};
            // radius <= deviateRadiusSq for second roll onwards
            const bool isInSmallCircle{radiusSq <= secoDevRadiusSq};

            // Pos outside big circle don't need to be checked as they re-roll to secShots
            if (!isInBigCircle)
            {
                ++posOutCircle;
                continue;
            }
            ++posInCircle;

            for (int z{bigZLower}; z <= bigZUpper; ++z)
            {
                shotPos.z = z;
                const bool shotHit{CylIntersect(pos, shotPos, diameter, halfHeight)};

                ++priShots;
                if (shotHit)
                {
                    ++priHits;
                }
                // Positions outside the big inscribed circle get mapped to
                // other points, but with lowered deviation. This causes the
                // possible z-axis offsets to get lowered alongside the
                // deviateRadius to compare to.
                if (isInSmallCircle && smallZLower <= z && z <= smallZUpper)
                {
                    ++secShots;
                    if (shotHit)
                    {
                        ++secHits;
                    }
                }
            }
        }
    }
    // our final hitchance will be a weighted average of the shots inside the
    // big circle that land (as they are used as is) and the shots outside the
    // big circle that get mapped to the hitchance for shots inside the smaller
    // circle (as they essentially get substituted for a random one)
    double totalPosXY{static_cast<double>(posInCircle + posOutCircle)};
    double firstHitChance{static_cast<double>(priHits) / static_cast<double>(priShots)};
    double firstHitWeight{static_cast<double>(posInCircle) / totalPosXY};
    double secondHitChance{static_cast<double>(secHits) / static_cast<double>(secShots)};
    double secondHitWeight{static_cast<double>(posOutCircle) / totalPosXY};
    double finalHitChance{100.0 * (firstHitWeight * firstHitChance + secondHitWeight * secondHitChance)};
    return finalHitChance;
}

void CalcOneDistVanilla(const OpenXcom::Position& pos, const OpenXcom::Unit& unit, bool kneeling, int accuracy)
{
    std::vector<double> devHitChances(211);
    int devOffset{0};
    // if explicit accuracy is given, only test deviations for that accuracy
    if (accuracy != -1)
    {
        devOffset = accuracy;      // start deviation at the accuracy percent
        devHitChances.resize(101); // only need 101 items for single accuracy
    }

    const int zShift{OpenXcom::CalcZShiftVanilla(pos)};
    const double alienHeight{unit.GetHalfHeight(kneeling)};
    DevVals prevDevVals{0, 1, 1};
    for (double& devHitChance : devHitChances)
    {
        int dev{OpenXcom::CalcDeviation(zShift, devOffset)};
        ++devOffset;
        // use previous deviation values to only test shots not already tested
        prevDevVals = TestAllShotsVanilla(prevDevVals, pos, unit.diameter, alienHeight, dev);
        devHitChance = prevDevVals.numHits * 100.0 / prevDevVals.numShots;
    }
    // Calcs hit chances
    std::vector<double> accChances{CalcHitChances(devHitChances)};
    if (accuracy != -1)
    {
        // single accuracy => print to console
        Utils::PrintHitChance(pos, unit.race, accuracy, accChances[0]);
    }
    else
    {
        // accuracy sweep => write to output file
        Utils::WriteHitChancesToFile(pos, unit.race, accChances);
    }
}

void CalcOneDistUniform(const OpenXcom::Position& pos, const OpenXcom::Unit& unit, bool kneeling, int accuracy)
{
    std::vector<double> devHitChances(211);
    int devOffset{0};
    // if explicit accuracy is given, only test deviations for that accuracy
    if (accuracy != -1)
    {
        devOffset = accuracy;      // start deviation at the accuracy percent
        devHitChances.resize(101); // only need 101 items for single accuracy
    }

    int zShift{OpenXcom::CalcZShiftUniform(pos)};
    double alienHeight{unit.GetHalfHeight(kneeling)};
    double dev1Result{0.0};
    for (double& devHitChance : devHitChances)
    {
        int dev{OpenXcom::CalcDeviation(zShift, devOffset)};
        ++devOffset;
        // Reuse deviation 1 test (all deviation 1 are the same)
        if (dev1Result && dev == 1)
        {
            devHitChance = dev1Result;
            continue;
        }
        devHitChance = TestAllShotsUniform(pos, unit.diameter, alienHeight, dev);
        // Save deviation 1 results for reusability
        if (dev == 1)
        {
            dev1Result = devHitChance;
        }
    }
    // Calcs hit chances
    std::vector<double> accChances{CalcHitChances(devHitChances)};
    if (accuracy != -1)
    {
        // single accuracy => print to console
        Utils::PrintHitChance(pos, unit.race, accuracy, accChances[0]);
    }
    else
    {
        // accuracy sweep => write to output file
        Utils::WriteHitChancesToFile(pos, unit.race, accChances);
    }
}

std::vector<double> CalcHitChances(const std::vector<double>& devHitChances)
{
    // Vector is either 1 item for single accuracy or 111 items for full accuracy sweep.
    std::vector<double> hitChances(devHitChances.size() - Constants::MOV_AVG_WNDW + 1);
    auto firstVal{devHitChances.begin()};
    auto lastVal{devHitChances.begin() + Constants::MOV_AVG_WNDW};
    for (double& hitChance : hitChances)
    {
        hitChance = std::accumulate(firstVal, lastVal, 0.0) / Constants::MOV_AVG_WNDW;
        ++firstVal;
        ++lastVal; // this shouldn't go out of range as we resize hitchances correctly
    }
    return hitChances;
}
} // namespace AccuracyTest
