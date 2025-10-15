from Structs import Position

def printHitChance(dist: Position, race: str, acc: int, hitChance: float) -> None:
    print(
        f"The hit chance for shooting a(n) {race} {dist} tiles away with an accuracy of {acc}% is: {hitChance:.1f}%")


def writeHitChancesToFile(dist: Position, race: str, hitChances: list[float]) -> None:
    with open(".\\output.csv", "w") as csvFile:
        csvFile.write(f"{race} at {dist}: ")
        for hitChance in hitChances:
            csvFile.write(f"{hitChance:.1f}%,")
        csvFile.write("\n")
    print(f"{race} at {dist} Done!")
