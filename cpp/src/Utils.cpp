#include "Utils.h"
#include "Accuracy.h"
#include "Structs.h"
#include <format>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

namespace Utils
{
void PrintHitChance(const OpenXcom::Position& pos, const std::string& race, int accuracy, double hitChance)
{
    std::cout << "The hit chance for shooting a(n) " << race << ' '
              << pos << " tiles away with an accuracy of " << accuracy
              << "% is: " << std::fixed << std::setprecision(1) << hitChance << "%\n"
              << std::endl;
}
// Calculates the hit chances per accuracy and writes them to a .csv
void WriteHitChancesToFile(const OpenXcom::Position& pos, const std::string& race, const std::vector<double>& hitChances)
{
    std::ofstream csvFile(".\\output.csv");
    csvFile << race << " at " << pos << ": ";
    csvFile << std::fixed << std::setprecision(1);
    for (auto&& hitChance : hitChances)
    {
        csvFile << hitChance << "%,";
    }

    csvFile << "\n";
    csvFile.close();
    std::cout << race << " at " << pos << " Done!\n";
}
} // namespace Utils
