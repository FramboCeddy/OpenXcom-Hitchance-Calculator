#pragma once

namespace OpenXcom
{
    struct Unit;
}

namespace Constants
{
    inline constexpr int TILE_WIDTH = 16;
    inline constexpr int TILE_HEIGHT = 24;
    inline constexpr int MOV_AVG_WNDW = 101;
    extern OpenXcom::Unit SECTOID;
    extern OpenXcom::Unit MUTON;
    extern OpenXcom::Unit ETHEREAL;
    extern OpenXcom::Unit XCOMSOLDIER;
    extern OpenXcom::Unit SECTOPOD;
    extern OpenXcom::Unit CYBERDISC;
    extern OpenXcom::Unit SNAKEMAN;
    extern OpenXcom::Unit CELATID;
    extern OpenXcom::Unit SILACOID;
    enum Direction
    {
        XAxis,
        YAxis,
        ZAxis,
        Diagonal
    };
}
