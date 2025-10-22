#include "Accuracy.h"
#include "Structs.h"
#include "cli.h"
#include <iostream>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    // no args provided
    if (argc < 2)
    {
        cli::PrintHelp(argv[0]);
        return EXIT_FAILURE;
    }
    const auto [pos, target, mode, kneel, accuracy] = cli::ParseCLI(argc, argv);
    if (!pos.isValid())
    {
        std::cerr << "Error: Coordinate must be positive integers and not all 0.\n";
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
