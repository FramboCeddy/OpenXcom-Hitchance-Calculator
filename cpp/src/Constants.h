#pragma once
namespace OpenXcom
{
struct Unit; // Forward declare unit struct (avoids circular include)
}

namespace Constants
{
inline constexpr int TILE_WIDTH = 16;
inline constexpr int TILE_HEIGHT = 24;
inline constexpr int MOV_AVG_WNDW = 101;
extern OpenXcom::Unit SECTOID;
extern OpenXcom::Unit MUTON;
extern OpenXcom::Unit ETHEREAL;
extern OpenXcom::Unit SOLDIER;
extern OpenXcom::Unit SECTOPOD;
extern OpenXcom::Unit CYBERDISC;
extern OpenXcom::Unit SNAKEMAN;
extern OpenXcom::Unit CELATID;
extern OpenXcom::Unit SILACOID;
extern OpenXcom::Unit TANK;
extern OpenXcom::Unit HOVERTANK;
} // namespace Constants
