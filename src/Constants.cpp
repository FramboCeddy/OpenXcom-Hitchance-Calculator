#include "Constants.h"
#include "Structs.h"

namespace Constants
{
    OpenXcom::Unit SECTOID{"Sectoid", 5, 16, 12};
    OpenXcom::Unit MUTON{"Muton", 7, 21, 16}; // + Chryssalid + Floater + Civilian
    OpenXcom::Unit ETHEREAL{"Ethereal", 7, 20, 15};
    OpenXcom::Unit XCOMSOLDIER{"XCOM Soldier", 7, 22, 14};
    OpenXcom::Unit SECTOPOD{"Sectopod", 32, 23, 16}; // + Reaper
    OpenXcom::Unit CYBERDISC{"Cyberdisc", 32, 15, 15};
    OpenXcom::Unit SNAKEMAN{"Snakeman", 7, 18, 18}; // + Zombie
    OpenXcom::Unit CELATID{"Celatid", 7, 12, 12};
    OpenXcom::Unit SILACOID{"Silacoid", 11, 10, 10};
    // TODO: turn this list of units into some external data structure like JSON and parse it.
}
