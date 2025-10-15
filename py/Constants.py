from Structs import Unit

TILE_WIDTH = 16
TILE_HEIGHT = 24
# both 0 and 100 are inclusive so 1 accuracy value consists of 101 deviations
MOVAVGWNDW = 101

SECTOID: Unit = Unit("Sectoid", 5, 16, 12)
MUTON: Unit = Unit("Muton", 7, 21, 16)
ETHEREAL: Unit = Unit("Ethereal", 7, 20, 15)
SOLDIER: Unit = Unit("XCOM Soldier", 7, 22, 14)
SECTOPOD: Unit = Unit("Sectopod", 32, 23, 16)
CYBERDISC: Unit = Unit("Cyberdisc", 32, 15, 15)
SNAKEMAN: Unit = Unit("Snakeman", 7, 18, 18)
CELATID: Unit = Unit("Celatid", 7, 12, 12)
SILACOID: Unit = Unit("Silacoid", 11, 10, 10)
TANK: Unit = Unit("XCOM Tank", 32, 16, 16)
HOVERTANK: Unit = Unit("XCOM Hovertank", 32, 12, 12)
