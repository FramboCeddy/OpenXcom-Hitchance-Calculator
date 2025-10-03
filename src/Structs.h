#pragma once
#include "Constants.h"
#include <format>
#include <ostream>
#include <string>

namespace OpenXcom
{
struct Position
{
    int x{0};
    int y{0};
    int z{0};
    Position(int xTile, int yTile, int zTile);
    void ToVoxel();
    bool isValid() const;
};
inline std::ostream& operator<<(std::ostream& ostream, const Position& pos)
{
    ostream << std::format("({}, {}, {})", pos.x / static_cast<double>(Constants::TILE_WIDTH), pos.y / static_cast<double>(Constants::TILE_WIDTH),
                           pos.z / static_cast<double>(Constants::TILE_HEIGHT));
    return ostream;
}

struct Unit
{
    std::string race{""};
    int diameter{1};
    Unit(std::string alienRace, int alienDiameter, int standHeight, int kneelHeight = -1);
    double GetHalfHeight(bool kneeling) const;

  private:
    double standHalfHeight{1};
    double kneelHalfHeight{1};
};
} // namespace OpenXcom

namespace AccuracyTest
{
struct DevVals
{
    // 0 Deviation is not possible to get however it still has
    // position (0, 0, 0) as a shot and that shot will hit.
    int deviation{0};
    int numHits{1};
    int numShots{1};
    DevVals(int dev, int numberHits, int numberShots);
};
enum class FiringMode
{
    Vanilla,
    Uniform,
    Angular,
};
} // namespace AccuracyTest
