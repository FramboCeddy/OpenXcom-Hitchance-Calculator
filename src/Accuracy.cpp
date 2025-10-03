#include "Accuracy.h"
#include "RNG.h"
#include "Structs.h"
#include "Utils.h"
#include "Xcom.h"
#include <cmath>
#include <numeric>

namespace AccuracyTest
{
constexpr bool PosInCircle(const OpenXcom::Position& pos, int diameter)
{
    return 4 * (pos.x * pos.x) + 4 * (pos.y * pos.y) < diameter * diameter;
}
bool CircleIntersect(const OpenXcom::Position& startPos, const OpenXcom::Position& endPos, int diameter)
{
    // If our shot position is within the circle, we will intersect
    if (PosInCircle(endPos, diameter))
    {
        return true;
    }
    // Ax + By + C = 0
    int lineA{startPos.y - endPos.y};
    int lineB{endPos.x - startPos.x};
    int lineC{startPos.x * endPos.y - endPos.x * startPos.y};

    // if our shot is parallel to z axis return true so we dont skip shots
    // FIXME: this really should not return true just so we don't early skip
    if (!lineA && !lineB)
    {
        return true;
    }
    double lineDistSq{static_cast<double>(lineC * lineC) / static_cast<double>(lineA * lineA + lineB * lineB)};
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
    int A{dx * dx + dy * dy};
    int B{2 * (startPos.x * dx + startPos.y * dy)};
    int Cmult4{(4 * (startPos.x * startPos.x) + 4 * (startPos.y * startPos.y) - (diameter * diameter))};

    // Shot parallel to Z-axis => Intersection occurs if shot is inside circle
    if (!A)
    {
        return isInCircle;
    }

    // shot has some XY direction
    // see if our shot in XY plane intersects the circle
    int D{B * B - A * Cmult4};
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
    Also takes in results from a previous test to skip an inner box and only
    test a hollow box. The benefit is that the hit chance is calculated and
    therefore has no variance. */
DevVals TestAllShotsVanilla(const DevVals& prevTest, const OpenXcom::Position& shooterPos, int alienDiameter, double alienHalfHeight, int deviation)
{
    if (deviation == prevTest.deviation)
    {
        // the boxes will be exactly the same so return previous results
        return prevTest;
    }
    // figure out if previous deviation is bigger or smaller
    const bool deviationIncreased{deviation > prevTest.deviation};
    const int bigBoxDev{deviationIncreased ? deviation : prevTest.deviation};
    const int smallBoxDev{deviationIncreased ? prevTest.deviation : deviation};
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
            if (!CircleIntersect(shooterPos, shotPos, alienDiameter))
            {
                continue;
            }
            for (int z{bigZLower}; z <= bigZUpper; ++z)
            {
                const bool InsideSmallBoxZ{smallZLower <= z && z <= smallZUpper};
                //  this position falls within our small box so is already
                //  tested
                if (InsideSmallBoxX && InsideSmallBoxY && InsideSmallBoxZ)
                {
                    continue;
                }
                shotPos.z = z;
                if (CylIntersect(shooterPos, shotPos, alienDiameter, alienHalfHeight))
                {
                    ++countHits;
                }
            }
        }
    }
    if (deviationIncreased)
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

double TestAllShotsUniform(const OpenXcom::Position& shooterPos, int alienDiameter, double alienHalfHeight, int deviation)
{
    // This is an intentional narrowing conversion as OXCE source code uses
    // deviation *= 0.85 (and deviation holds an int)
    const int secondaryDeviation{static_cast<int>(deviation * 0.85)};
    const int deviateRadiusSq{(deviation / 2) * (deviation / 2)};
    const int secondaryDeviateRadiusSq{(secondaryDeviation / 2) * (secondaryDeviation / 2)};
    const int bigXYLower{-deviation / 2};
    const int bigXYUpper{deviation - deviation / 2};
    const int bigZLower{-deviation / 8};
    const int bigZUpper{(deviation / 2) / 2 - deviation / 8};
    // if result gets shifted it lowers deviation in place and afterwards
    // calculates a dz with this lowered deviation
    const int smallZLower{-secondaryDeviation / 8};
    const int smallZUpper{(secondaryDeviation / 2) / 2 - secondaryDeviation / 8};
    int firstHits{0};
    int firstShots{0};
    int secondHits{0};
    int secondShots{0};
    int posOutsideCircle{0};             // dx and dy fall outside circle
    int posInsideCircle{0};              // dx and dy fall inside circle so get used
    OpenXcom::Position shotPos{0, 0, 0}; // will be filled in later
    for (int x{bigXYLower}; x <= bigXYUpper; ++x)
    {
        shotPos.x = x;
        for (int y{bigXYLower}; y <= bigXYUpper; ++y)
        {
            shotPos.y = y;
            const int radiusSq{x * x + y * y};
            // radiusSq <= deviateRadiusSq for first roll
            const bool isInBigCircle{radiusSq <= deviateRadiusSq};
            // radius <= deviateRadiusSq for second roll onwards
            const bool isInSmallCircle{radiusSq <= secondaryDeviateRadiusSq};

            // Pos outside big circle don't need to be checked as they dont
            // succeed on first attempt and second attempts need to be in small
            // circle (which is smaller or equal size)
            if (!isInBigCircle)
            {
                ++posOutsideCircle;
                continue;
            }
            ++posInsideCircle;

            for (int z{bigZLower}; z <= bigZUpper; ++z)
            {
                shotPos.z = z;
                bool shotHit{CylIntersect(shooterPos, shotPos, alienDiameter, alienHalfHeight)};
                ++firstShots;
                if (shotHit)
                {
                    ++firstHits;
                }
                // Positions outside the big inscribed circle get mapped to
                // other points, but will lower the deviation. This causes the
                // possible z-axis deviation to also get lowered alongside the
                // deviateRadius to compare to.
                if (isInSmallCircle && smallZLower <= z && z <= smallZUpper)
                {
                    ++secondShots;
                    if (shotHit)
                    {
                        ++secondHits;
                    }
                }
            }
        }
    }

    // our final hitchance will be a weighted average of the shots inside the
    // big circle that land (as they are used as is) and the shots outside the
    // big circle that get mapped to the hitchance for shots inside the smaller
    // circle (as they essentially get substituted for a random one)
    double totalPosXY{static_cast<double>(posInsideCircle + posOutsideCircle)};
    double firstHitChance{static_cast<double>(firstHits) / static_cast<double>(firstShots)};
    double firstHitWeight{static_cast<double>(posInsideCircle) / totalPosXY};
    double secondHitChance{static_cast<double>(secondHits) / static_cast<double>(secondShots)};
    double secondHitWeight{static_cast<double>(posOutsideCircle) / totalPosXY};
    double finalHitChance{100.0 * (firstHitWeight * firstHitChance + secondHitWeight * secondHitChance)};
    return finalHitChance;
}

void CalcOneDistVanilla(const OpenXcom::Position& distance, const OpenXcom::Unit& alien, bool kneeling, int accuracy)
{
    std::vector<double> devHitChances(101);
    int i{0};
    // if explicit accuracy is given, only test deviations for that accuracy
    if (accuracy != -1)
    {
        i = accuracy;
    }
    // else we test the entire accuracy range
    else
    {
        devHitChances.resize(211);
    }

    const int zShift{OpenXcom::CalcZShiftVanilla(distance)};
    const double alienHeight{alien.GetHalfHeight(kneeling)};
    DevVals prevDevVals{0, 1, 1};
    for (double& devHitChance : devHitChances)
    {
        int dev{(i <= 100) ? 150 - i : 110 - i};
        dev = std::max(1, zShift * dev / 200);
        ++i;
        // uses previous deviation values to only test shots not already
        // included
        prevDevVals = TestAllShotsVanilla(prevDevVals, distance, alien.diameter, alienHeight, dev);
        devHitChance = prevDevVals.numHits * 100.0 / prevDevVals.numShots;
    }
    // Calcs hit chances
    std::vector<double> accChances{CalcHitChances(devHitChances)};
    if (accuracy != -1)
    {
        // single accuracy -> print to console
        Utils::printHitChance(distance, alien.race, accuracy, accChances[0]);
    }
    else
    {
        // accuracy sweep -> write to output file
        Utils::writeHitChancesToFile(distance, alien.race, accChances);
    }
}

void CalcOneDistUniform(const OpenXcom::Position& distance, const OpenXcom::Unit& alien, bool kneeling, int accuracy)
{
    std::vector<double> devHitChances(101);
    int i{0};
    // if explicit accuracy is given, only test deviations for that accuracy
    if (accuracy != -1)
    {
        i = accuracy;
    }
    // else we test the entire accuracy range
    else
    {
        devHitChances.resize(211);
    }

    int zShift{OpenXcom::CalcZShiftUniform(distance)};
    double alienHeight{alien.GetHalfHeight(kneeling)};
    double dev1Result{0.0};
    for (double& devHitChance : devHitChances)
    {
        // testing all deviations as opposed to RNG'ing them
        int dev{(i <= 100) ? 150 - i : 110 - i};
        dev = std::max(1, zShift * dev / 200);
        ++i;
        // Reuse deviation 1 test (all deviation 1 are the same)
        if (dev1Result && dev == 1)
        {
            devHitChance = dev1Result;
            continue;
        }
        devHitChance = TestAllShotsUniform(distance, alien.diameter, alienHeight, dev);
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
        // single accuracy -> print to console
        Utils::printHitChance(distance, alien.race, accuracy, accChances[0]);
    }
    else
    {
        // accuracy sweep -> write to output file
        Utils::writeHitChancesToFile(distance, alien.race, accChances);
    }
}
std::vector<double> CalcHitChances(const std::vector<double>& devHitChances)
{
    // Vector is either 1 item for single accuracy or 111 items for full accuracy sweep.
    std::vector<double> hitChances(devHitChances.size() - Constants::MOV_AVG_WNDW + 1);
    auto firstVal{devHitChances.begin()};
    auto lastVal{devHitChances.begin() + Constants::MOV_AVG_WNDW};
    for (auto& hitChance : hitChances)
    {
        hitChance = std::accumulate(firstVal, lastVal, 0.0) / Constants::MOV_AVG_WNDW;
        ++firstVal;
        ++lastVal; // this shouldn't go out of range as we resize hitchances correctly
    }
    return hitChances;
}
} // namespace AccuracyTest
namespace AngularFiringSpread
{
int CalcZShiftAngular(const OpenXcom::Position& Pos)
{
    // Euclidean distance but casted to int to stay on the same level as OXCE ZShift
    // NOTE: It is probably possible to use the squared distance to avoid a sqrt call
    return static_cast<int>(std::sqrt(Pos.x * Pos.x + Pos.y * Pos.y + Pos.z * Pos.z));
}

int CalcDeviationAngular(int zShift, double accuracy)
{
    // Remove the binary +50 or +10 from deviation
    int deviation = OpenXcom::RNG::generate(0, 100) - (accuracy * 100);
    // we first max deviation to 1 then multiply by zshift to keep long range shots having a little bit of offset
    deviation = std::max(1, deviation);
    return zShift * deviation / 200;
}

OpenXcom::Position XCOMAccuracyAngular(const OpenXcom::Position& Pos, double accuracy)
{
    int zShift = CalcZShiftAngular(Pos);
    int deviation = CalcDeviationAngular(zShift, accuracy);
    return OpenXcom::CalcShotOffsetUniform(deviation);
}

} // namespace AngularFiringSpread
