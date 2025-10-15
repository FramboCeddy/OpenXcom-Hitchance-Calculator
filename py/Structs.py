# Can't import from Constants due to circular import
TILE_WIDTH = 16
TILE_HEIGHT = 24

class Position:
    def __init__(self, x: int, y: int, z: int) -> None:
        self.x: int = x
        self.y: int = y
        self.z: int = z

    def __repr__(self) -> str:
        return f"({self.x // TILE_WIDTH}, {self.y // TILE_WIDTH}, {self.z // TILE_HEIGHT})"

    def isValid(self) -> bool:
        return (self.x >= 0 and self.y >= 0 and self.z >= 0 and not (self.x == 0 and self.y == 0 and self.z == 0))

class Unit:
    def __init__(self, race: str, diameter: int, standHeight: int, kneelHeight: int) -> None:
        self.race: str = race
        self.diameter: int = diameter
        self.standHalfHeight: float = standHeight / 2
        self.kneelHalfHeight: float = kneelHeight / 2
        self.kneeling: bool = False

    def GetHalfHeight(self, kneeling: bool) -> float:
        return self.kneelHalfHeight if kneeling else self.standHalfHeight
