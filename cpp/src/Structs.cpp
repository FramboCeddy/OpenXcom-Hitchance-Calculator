#include "Structs.h"
#include <ostream>
#include <string>

namespace OpenXcom
{
Position::Position(int xVoxel, int yVoxel, int zVoxel)
    : x{xVoxel}
    , y{yVoxel}
    , z{zVoxel}
{
}

bool Position::isValid() const
{
    return (x >= 0 && y >= 0 && z >= 0 && !(x == 0 && y == 0 && z == 0));
}

Unit::Unit(std::string alienRace, int alienDiameter, int standHeight, int kneelHeight)
    : race{alienRace}
    , diameter{alienDiameter}
    , standHalfHeight{standHeight / 2.0}
    , kneelHalfHeight{(kneelHeight == -1) ? standHeight / 2.0 : kneelHeight / 2.0}
{
}
double Unit::GetHalfHeight(bool kneeling) const
{
    return kneeling ? kneelHalfHeight : standHalfHeight;
}
} // namespace OpenXcom

namespace AccuracyTest
{
DevVals::DevVals(int dev, int numberHits, int numberShots)
    : deviation{dev}
    , numHits{numberHits}
    , numShots{numberShots}
{
}
} // namespace AccuracyTest
