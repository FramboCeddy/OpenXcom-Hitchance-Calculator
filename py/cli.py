import Constants
from sys import stderr, exit
from Structs import Unit, Position

type cliArgs = tuple[Position, Unit, str, bool, int]


# Helper: trim whitespace from both ends
def TrimWhitespace(string: str) -> str:
    stripped = string.strip()
    return stripped

def PrintHelp(progName: str) -> None:
    print(f"Usage: python {progName} (--dist | -d) <x,y,z> [--acc | -a <integer>]\n")
    print("Options:")
    print("  --help,       -h                      | Show this help message")
    print("  --listunits,  -l                      | Show all units that can be used.")
    print("  --dist,       -d <x,y,z>              | 3D distance between the shooter and target in tiles as x,y,z (required")
    print("  --unit,       -u <name>               | Name of the unit you are aiming at. (default: Muton)")
    print("  --kneel,      -k <true|false>         | Is the unit you are aiming at kneeling or not. (default: false)")
    print("  --mode,       -m <vanilla|uniform>    | Chooses the spread model that you wish to test. (default: vanilla)")
    print("  --acc,        -a <N>                  | Accuracy value in whole percents as seen on the crosshair. (default: omitted)")
    print("                                        | If omitted, runs a full accuracy sweep of [0%, 110%] and writes to a file named output.csv")
    print(flush=True)

def ListUnits() -> None:
    print("List of all available units:")
    print("celatid,\nchryssalid (= muton),\ncivilian (= muton),\ncyberdisc,\nethereal,\nfloater (= muton),\nhovertank,\nmuton,\nreaper (= sectopod),\nsectoid,\nsectopod,\nsilacoid,\nsoldier,\ntank,")
    print(flush=True)

def ParseUnit(unitStr: str) -> Unit:
    lowerStr = unitStr.lower()
    if (lowerStr == "sectoid"):
        return Constants.SECTOID
    if (lowerStr in ("muton", "floater", "chryssalid", "civilian")):
        return Constants.MUTON
    if (lowerStr in ("snakeman", "zombie")):
        return Constants.SNAKEMAN
    if (lowerStr == "ethereal"):
        return Constants.ETHEREAL
    if (lowerStr in ("sectopod", "reaper")):
        return Constants.SECTOPOD
    if (lowerStr == "soldier"):
        return Constants.SOLDIER
    if (lowerStr == "cyberdisc"):
        return Constants.CYBERDISC
    if (lowerStr == "celatid"):
        return Constants.CELATID
    if (lowerStr == "silacoid"):
        return Constants.SILACOID
    if (lowerStr == "tank"):
        return Constants.TANK
    if (lowerStr == "hovertank"):
        return Constants.HOVERTANK
    # muton by default as most units are 7 wide and ~20 high
    return Constants.MUTON

def ParsePosition(coordStr: str) -> Position:
    try:
        x,y,z = (int(TrimWhitespace(i)) for i in coordStr.split(","))
        return Position(x*Constants.TILE_WIDTH, y*Constants.TILE_WIDTH, z*Constants.TILE_HEIGHT)
    except:
        print(f"Invalid coordinate value", file=stderr)
        return Position(-1, -1, -1)

def ParseAccuracy(accStr: str) -> int:
    accuracy: int = -1
    try:
        accuracy = int(TrimWhitespace(accStr))
        return accuracy
    except:
        print(f"Invalid accuracy: {accStr} - must be a positive whole number", file=stderr)
        return -1

def ParseCLI(args) -> cliArgs:
    # default values
    pos :Position = Position(-1, -1, -1)
    target: Unit = Constants.MUTON
    mode: str = "vanilla"
    kneeling: bool = False
    accuracy:int = -1
    # Manual parser
    argc = len(args)
    for argi, argv in enumerate(args, 0):
        if argv == "--help" or argv == "-h":
            PrintHelp(args[0])
            exit(0)
        elif argv == "--listunits" or argv == "-l":
            ListUnits()
            exit(0)
        elif (argv == "--mode" or argv == "-m") and argi + 1 < argc:
            modeStr: str = args[argi+1].lower()
            if modeStr == "uniform":
                mode = "uniform"
            elif (str == "vanilla"):
                mode = "vanilla"
        elif(argv == "--dist" or argv == "-d") and argi + 1 < argc:
            pos = ParsePosition(args[argi + 1])
        elif ((argv == "--acc" or argv == "-a") and argi + 1 < argc):
            accuracy = ParseAccuracy(args[argi + 1])
        elif ((argv == "--kneel" or argv == "-k") and argi + 1 < argc):
            boolStr = args[argi + 1].lower()
            if boolStr == "true":
                kneeling = True
            else:
                kneeling = False
        elif (argv == "--unit" or argv == "-u") and argi + 1 < argc:
            target = ParseUnit(args[argi + 1])

    return (pos, target, mode, kneeling, accuracy)
