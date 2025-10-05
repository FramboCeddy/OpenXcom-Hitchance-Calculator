#include "Accuracy.h"
#include "Structs.h"
#include "Utils.h"
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
