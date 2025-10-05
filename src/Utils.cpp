#include "Utils.h"
#include "Accuracy.h"
#include "Structs.h"
#include <format>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <vector>

namespace Utils
{
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
