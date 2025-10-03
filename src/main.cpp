#include "main.h"
#include "Accuracy.h"
#include "Structs.h"
#include "Utils.h"
#include "Xcom.h"
#include "cli.h"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <vector>

int main(int argc, char* argv[])
{
    // no args provided
    if (argc < 2)
    {
        cli::printHelp(argv[0]);
        return EXIT_FAILURE;
    }
    auto [pos, target, mode, kneel, accuracy] = cli::parseCLI(argc, argv);
    if (!pos.isValid())
    {
        std::cerr << "Error: Coordinate must be positive integers and not all 0." << std::endl;
        return EXIT_FAILURE;
    }
    switch (mode)
    {
    case AccuracyTest::FiringMode::Vanilla:
        AccuracyTest::CalcOneDistVanilla(pos, target, kneel, accuracy);
        return EXIT_SUCCESS;
    case AccuracyTest::FiringMode::Uniform:
        AccuracyTest::CalcOneDistUniform(pos, target, kneel, accuracy);
        return EXIT_SUCCESS;
    default:
        std::cerr << "ERROR: wrong firingmode\n";
        return EXIT_FAILURE;
    }
}
void TestImplementation()
{
    int testCount = 10'000'000;
    int countHit = 0;
    bool kneel = false;
    std::vector<int> accuracies{0, 25, 45, 75, 110};
    std::vector<OpenXcom::Position> distances{
        OpenXcom::Position(5 * 16, 0, 0),
        OpenXcom::Position(0, 10 * 16, 0),
        OpenXcom::Position(7 * 16, 7 * 16, 0),
        OpenXcom::Position(0, 0, 2 * 24)};

    for (auto&& acc : accuracies)
    {
        for (auto&& dist : distances)
        {
            // reset test
            countHit = 0;
            // Dumb Test
            for (int i = 0; i < testCount; i++)
            {
                OpenXcom::Position shotPos = OpenXcom::XCOMAccuracyUniform(dist, acc / 100.0);
                if (AccuracyTest::CylIntersect(dist, shotPos, Constants::MUTON.diameter, Constants::MUTON.GetHalfHeight(kneel)))
                {
                    ++countHit;
                }
            }
            std::cout << "Testing " << dist << " distance and " << acc << " accuracy\n";
            std::cout << "Dumb test HC%: " << countHit * 100.0 / testCount << '\n';
            // Smart Test
            AccuracyTest::CalcOneDistUniform(dist, Constants::MUTON, kneel, acc);
        }
    }
}
