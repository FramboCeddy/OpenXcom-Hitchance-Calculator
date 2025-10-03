#include "Utils.h"
#include "Accuracy.h"
#include "Constants.h"
#include "Structs.h"
#include <chrono>
#include <format>
#include <fstream>
#include <iostream>
#include <numeric>
#include <vector>

namespace Utils
{
/*
Add Positions in X,Y,Z axis or diagonal line from StepSize to maxDistance with a spacing of StepSize in tiles
*/
void AddDistances(std::vector<OpenXcom::Position>& distanceVector, Constants::Direction direction, int maxDistance, int stepSize)
{
    int x{0};
    int y{0};
    int z{0};
    // x,y,z are the directionality constants
    switch (direction)
    {
    case Constants::Direction::XAxis:
        x = Constants::TILE_WIDTH;
        break;
    case Constants::Direction::YAxis:
        y = Constants::TILE_WIDTH;
        break;
    case Constants::Direction::ZAxis:
        z = Constants::TILE_HEIGHT;
        break;
    case Constants::Direction::Diagonal:
        x = Constants::TILE_WIDTH;
        y = Constants::TILE_WIDTH;
        break;
    default:
        break;
    }
    for (int i{stepSize}; i <= maxDistance; i += stepSize)
    {
        // x,y,z scaling is set using the direction in the switch cases.
        distanceVector.emplace_back(i * x, i * y, i * z);
    }
}

void printHitChance(const OpenXcom::Position& distance, const std::string& alienRace, int accuracy, double hitChance)
{
    std::cout << "The hit chance for shooting at a(n) " << alienRace << ' '
              << distance << " tiles away with an accuracy of " << accuracy
              << "% is: " << std::fixed << std::setprecision(1) << hitChance << "%\n"
              << std::endl;
}
// Calculates the hit chances per accuracy and writes them to a .csv
void writeHitChancesToFile(const OpenXcom::Position& distance, const std::string& alienRace, const std::vector<double>& hitChances)
{
    std::ofstream csvFile(".\\output.csv");
    csvFile << alienRace << " at " << distance << ": ";
    csvFile << std::fixed << std::setprecision(1);
    for (auto&& hitChance : hitChances)
    {
        csvFile << hitChance << "%,";
    }

    csvFile << "\n";
    csvFile.close();
    std::cout << alienRace << " at " << distance << " Done!\n";
}
} // namespace Utils
