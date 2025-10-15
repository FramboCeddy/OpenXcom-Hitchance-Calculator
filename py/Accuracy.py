from math import sqrt
from Structs import Position, Unit
from Constants import MOVAVGWNDW
from Utils import printHitChance, writeHitChancesToFile

# NOTE: anywhere OXC uses operator/ with 2 int operands it will truncate the value (~round to 0)
# while Python operator/ with 2 int operands will not truncate and leave it as a double/float value.
# In order to keep the same behaviour there are 2 options:
# 1. Cast back to an int value after every division
# This is ugly to see and annoying to type as there are a LOT of divisions in here
# 2. use python's operator// (this does NOT truncate but round to -inf)
# This means behaviour is different for negative values.
# So where we know a value will be positive, we will be using the operator//

def CalcZShiftVanilla(Pos: Position) -> int:
    xDist: int = abs(Pos.x)
    yDist: int = abs(Pos.y)
    zDist: int = abs(Pos.z)
    xyShift: int = 0
    zShift: int = 0

    if xDist // 2 <= yDist:
        xyShift = xDist // 4 + yDist
    else:
        xyShift = (xDist + yDist) // 2

    if xyShift <= zDist:
        zShift = xyShift // 2 + zDist
    else:
        zShift = xyShift + zDist // 2
    return zShift


def CalcZShiftUniform(Pos: Position) -> int:
    xDist: int = abs(Pos.x)
    yDist: int = abs(Pos.y)
    zDist: int = abs(Pos.z)
    xyShift: int = 0
    zShift: int = 0

    if xDist <= yDist:
        xyShift = xDist // 4 + yDist
    else:
        xyShift = xDist + yDist // 4
    xyShift = int(xyShift * 0.839)

    if xyShift <= zDist:
        zShift = xyShift // 2 + zDist
    else:
        zShift = xyShift + zDist // 2
    return zShift


def CalcDeviation(zShift: int, deviation: int) -> int:
    if deviation <= 100:
        return max(1, zShift * (150 - deviation) // 200)
    else:
        return max(1, zShift * (110 - deviation) // 200)


def PosInCircle(pos: Position, diameter: int) -> bool:
    return 4 * (pos.x * pos.x) + 4*(pos.y * pos.y) < diameter * diameter


def CircleIntersect(startPos: Position, endPos: Position, diameter: int) -> bool:
    isInCircle: bool = PosInCircle(endPos, diameter)
    if isInCircle:
        return True

    a: int = startPos.y - endPos.y
    b: int = endPos.x - startPos.x
    c: int = startPos.x * endPos.y - endPos.x * startPos.y

    if a == 0 and b == 0:
        return isInCircle
    lineDistSq: float = (c*c) / (a*a+b*b)
    return 4 * lineDistSq < diameter * diameter


def CylIntersect(startPos: Position, endPos: Position, diameter: int, halfHeight: float) -> bool:
    isInCircle: bool = PosInCircle(endPos, diameter)
    isWithinHeight: bool = abs(endPos.z) < halfHeight
    if isInCircle and isWithinHeight:
        return True

    dx: int = endPos.x - startPos.x
    dy: int = endPos.y - startPos.y
    dz: int = endPos.z - startPos.z

    a = dx * dx + dy * dy
    b = 2 * (startPos.x * dx + startPos.y * dy)
    cMult4 = 4*(startPos.x * startPos.x) + 4 *(startPos.y * startPos.y) - (diameter * diameter)

    if a == 0:
        return isInCircle

    discr: int = b*b - a*cMult4
    if discr <= 0:
        return False

    sqrt_Discr: float = sqrt(discr)
    root1: float = (-b - sqrt_Discr) / (2*a)
    root2: float = (-b + sqrt_Discr) / (2*a)
    z1: float = startPos.z + root1 * dz
    z2: float = startPos.z + root2 * dz

    if (halfHeight < z1 and halfHeight < z2) or (z1 < -halfHeight and z2 < -halfHeight):
        return False

    return True

def TestAllShotsVanilla(prevTest: tuple[int, int, int], shooterPos: Position, diameter: int, halfHeight: float, dev: int) -> tuple[int, int, int]:
    prevTestDev = prevTest[0]
    prevTestNumHits = prevTest[1]
    if dev == prevTestDev:
        return prevTest

    devIncreased: bool = dev > prevTestDev
    bigBoxDev: int = dev if devIncreased else prevTestDev
    smallBoxDev: int = prevTestDev if devIncreased else dev
    bigXYLower: int = -(bigBoxDev // 2)
    bigXYUpper: int = bigBoxDev - bigBoxDev // 2
    bigZLower: int = -(bigBoxDev // 8)
    bigZUpper: int = (bigBoxDev // 2) // 2 - bigBoxDev // 8
    smallXYLower: int = -(smallBoxDev // 2)
    smallXYUpper: int = smallBoxDev - smallBoxDev // 2
    smallZLower: int = -(smallBoxDev // 8)
    smallZUpper: int = (smallBoxDev // 2) // 2 - smallBoxDev // 8
    countHits: int = 0
    shotPos: Position = Position(0, 0, 0)
    for x in range(bigXYLower, bigXYUpper + 1):  # + 1 for inclusive end
        shotPos.x = x
        InsideSmallBoxX: bool = smallXYLower <= x and x <= smallXYUpper
        for y in range(bigXYLower, bigXYUpper + 1):
            shotPos.y = y
            InsideSmallBoxY: bool = smallXYLower <= y and y <= smallXYUpper
            if not CircleIntersect(shooterPos, shotPos, diameter):
                continue

            for z in range(bigZLower, bigZUpper + 1):
                InsideSmallBoxZ: bool = smallZLower <= z and z <= smallZUpper
                if InsideSmallBoxX and InsideSmallBoxY and InsideSmallBoxZ:
                    continue

                shotPos.z = z
                if CylIntersect(shooterPos, shotPos, diameter, halfHeight):
                    countHits += 1
    if devIncreased:
        numShots: int = (bigBoxDev + 1) * (bigBoxDev + 1) * (bigBoxDev // 4 + 1)
        return (dev, prevTestNumHits + countHits, numShots)
    else:
        numShots: int = (smallBoxDev + 1) * (smallBoxDev + 1) * (smallBoxDev // 4 + 1)
        return (dev, prevTestNumHits - countHits, numShots)


def TestAllShotsUniform(shooterPos: Position, diameter: int, halfHeight: float, dev: int) -> float:
    secDev: int = int(dev * 0.85)
    priDevRadiusSq: int = (dev // 2) * (dev // 2)
    secDevRadiusSq: int = (secDev // 2) * (secDev // 2)
    bigXYLower: int = -(dev // 2)
    bigXYUpper: int = dev - (dev // 2)
    bigZLower: int = -(dev // 8)
    bigZUpper: int = (dev // 2) // 2 - (dev // 8)
    smallZLower: int = -(secDev // 8)
    smallZUpper: int = (secDev // 2) // 2 - (secDev // 8)
    priHits: int = 0
    priShots: int = 0
    secHits: int = 0
    secShots: int = 0
    posOutCircle: int = 0
    posInCircle: int = 0
    shotPos: Position = Position(0, 0, 0)

    for x in range(bigXYLower, bigXYUpper + 1):  # + 1 for inclusive end
        shotPos.x = x
        for y in range(bigXYLower, bigXYUpper + 1):
            shotPos.y = y
            radiusSq: int = x*x + y*y
            isInBigCircle: bool = radiusSq <= priDevRadiusSq
            isInSmallCircle: bool = radiusSq <= secDevRadiusSq

            if not isInBigCircle:
                posOutCircle += 1
                continue
            posInCircle += 1

            for z in range(bigZLower, bigZUpper + 1):
                shotPos.z = z
                shotHit: bool = CylIntersect(shooterPos, shotPos, diameter, halfHeight)

                priShots += 1
                if shotHit:
                    priHits += 1

                if isInSmallCircle and smallZLower <= z and z <= smallZUpper:
                    secShots += 1
                    if shotHit:
                        secHits += 1

    totalPosXY: float = posInCircle + posOutCircle
    priHitChance: float = priHits / (priShots)
    priHitWeight: float = posInCircle / totalPosXY
    secHitChance: float = secHits / secShots
    secHitWeight: float = posOutCircle / totalPosXY
    finalHitChance: float = 100.0 * (priHitWeight * priHitChance + secHitWeight * secHitChance)
    return finalHitChance


def CalcOneDistVanilla(dist: Position, alien: Unit, kneeling: bool, acc: int) -> list[float]:
    devHitChances: list[float] = [0]*101
    i: int = 0
    if acc != -1:
        i = acc
    else:
        devHitChances = [0]*211

    zShift: int = CalcZShiftVanilla(dist)
    alienHeight: float = alien.GetHalfHeight(kneeling)
    prevDevVals: tuple[int, int, int] = (0, 1, 1)
    for index, _ in enumerate(devHitChances):
        dev: int = 150 - i if i <= 100 else 110 - i
        dev = max(1, zShift * dev // 200)
        i += 1

        prevDevVals = TestAllShotsVanilla(
            prevDevVals, dist, alien.diameter, alienHeight, dev)
        devHitChances[index] = prevDevVals[1] * 100.0 / prevDevVals[2]

    accChances = CalcHitChances(devHitChances)
    if acc != -1:
        printHitChance(dist, alien.race, acc, accChances[0])
    else:
        writeHitChancesToFile(dist, alien.race, accChances)

    return accChances


def CalcOneDistUniform(dist: Position, alien: Unit, kneeling: bool, acc: int) -> list[float]:
    devHitChances: list[float] = [0]*101
    i: int = 0
    if acc != -1:
        i = acc
    else:
        devHitChances: list[float] = [0]*211

    zShift: int = CalcZShiftUniform(dist)
    alienHeight: float = alien.GetHalfHeight(kneeling)
    dev1Result: float = 0.0
    for index, _ in enumerate(devHitChances):
        dev: int = 150 - i if i <= 100 else 110 - i
        dev = max(1, zShift * dev // 200)
        i += 1

        if dev1Result != 0.0 and dev == 1:
            devHitChances[index] = dev1Result
            continue

        devHitChances[index] = TestAllShotsUniform(
            dist, alien.diameter, alienHeight, dev)
        if dev == 1:
            dev1Result = devHitChances[index]

    accChances = CalcHitChances(devHitChances)
    if acc != -1:
        printHitChance(dist, alien.race, acc, accChances[0])
    else:
        writeHitChancesToFile(dist, alien.race, accChances)

    return accChances

def CalcHitChances(devHitChances: list[float]) -> list[float]:
    hitChances: list[float] = [0] * (len(devHitChances) - MOVAVGWNDW + 1)

    for index, _ in enumerate(hitChances):
        sumChance: float = sum(devHitChances[index:index+MOVAVGWNDW])
        avgChance: float = sumChance / MOVAVGWNDW
        hitChances[index] = avgChance

    return hitChances
