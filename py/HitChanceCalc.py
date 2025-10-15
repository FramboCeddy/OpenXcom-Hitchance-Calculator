from sys import argv, exit, stderr
from cli import PrintHelp, ParseCLI
from Accuracy import CalcOneDistVanilla, CalcOneDistUniform


if __name__ == "__main__":
    argc: int = len(argv)
    if argc < 2:
        PrintHelp(argv[0])
        exit(1)
    pos, target, mode, kneel, accuracy = ParseCLI(argv)
    if not pos.isValid():
        print("Error: Coordinate must be positive integers and not all 0.", stderr)
        exit(1)

    match mode:
        case "vanilla":
            CalcOneDistVanilla(pos, target, kneel, accuracy)
            exit(0)
        case "uniform":
            CalcOneDistUniform(pos, target, kneel, accuracy)
            exit(0)
        case _:
            print("ERROR: wrong firingmode", stderr)
            exit(1)
