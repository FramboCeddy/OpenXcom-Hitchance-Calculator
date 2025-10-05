#include "cli.h"
#include "Constants.h"
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <vector>

// TODO: i REALLY need some kind of gui but i guess this cli works
namespace cli
{
// Helper: trim whitespace from both ends
std::string trim(const std::string& str)
{
    size_t first = str.find_first_not_of(" \t");
    size_t last = str.find_last_not_of(" \t");
    if (first == std::string::npos || last == std::string::npos)
        return "";
    return str.substr(first, last - first + 1);
}

void printHelp(const std::string& progName)
{
    std::cout << "Usage: " << progName << " (--pos | -p) <x,y,z> [--acc | -a <integer>]\n\n"
              << "Options:\n"
              << "  --help,       -h                      | Show this help message\n"
              << "  --listunits,  -l                      | Show all units that can be used.\n"
              << "  --dist,       -d <x,y,z>              | 3D distance between the shooter and target in tiles as x,y,z (required)\n"
              << "  --unit,       -u <name>               | Name of the unit you are aiming at. (default: Muton)\n"
              << "  --kneel,      -k <true|false>         | Is the unit you are aiming at kneeling or not. (default: false)\n"
              << "  --mode,       -m <Vanilla|Uniform>    | Chooses the spread model that you wish to test. (default: Vanilla)\n"
              << "  --acc,        -a <N>                  | Accuracy value in whole percents. (default: omitted)\n"
              << "                                        | If omitted, runs a full accuracy sweep of [0%, 110%] and writes to a file named output.csv\n"
              << std::flush;
}

void printUnits()
{
    std::cout << "List of all available units:\n"
              << "celatid,\nchryssalid (= muton),\ncivilian (= muton),\ncyberdisc,\nethereal,\nfloater (= muton),\nhovertank,\nmuton,\nreaper (= sectopod),\nsectoid,\nsectopod,\nsilacoid,\nsoldier,\ntank,\n"
              << std::flush;
}

OpenXcom::Unit parseUnit(std::string Str)
{
    std::transform(Str.begin(), Str.end(), Str.begin(), ::tolower);
    if (Str == "sectoid")
    {
        return Constants::SECTOID;
    }
    if (Str == "muton" || Str == "floater" || Str == "chryssalid" || Str == "civilian")
    {
        return Constants::MUTON;
    }
    if (Str == "snakeman" || Str == "zombie")
    {
        return Constants::SNAKEMAN;
    }
    if (Str == "ethereal")
    {
        return Constants::ETHEREAL;
    }
    if (Str == "sectopod" || Str == "reaper")
    {
        return Constants::SECTOPOD;
    }
    if (Str == "soldier")
    {
        return Constants::SOLDIER;
    }
    if (Str == "cyberdisc")
    {
        return Constants::CYBERDISC;
    }
    if (Str == "celatid")
    {
        return Constants::CELATID;
    }
    if (Str == "silacoid")
    {
        return Constants::SILACOID;
    }
    if (Str == "tank")
    {
        return Constants::TANK;
    }
    if (Str == "hovertank")
    {
        return Constants::HOVERTANK;
    }
    // muton by default as most units are 7 wide and ~20 high
    return Constants::MUTON;
}

OpenXcom::Position parsePosition(const std::string& coordStr)
{
    std::stringstream ss(coordStr);
    std::vector<int> coords;
    coords.reserve(3);
    std::string item;
    while (std::getline(ss, item, ','))
    {
        item = trim(item);
        try
        {
            coords.push_back(std::stoi(item));
        }
        catch (const std::invalid_argument&)
        {
            std::cerr << "Invalid coordinate value: " << item << "\n";
            return OpenXcom::Position{-1, -1, -1};
        }
    }
    return OpenXcom::Position{coords[0] * Constants::TILE_WIDTH,
                              coords[1] * Constants::TILE_WIDTH,
                              coords[2] * Constants::TILE_HEIGHT};
}

auto parseAccuracy(const std::string& accStr)
{
    int accuracy{};
    try
    {
        accuracy = std::stoi(trim(accStr));
    }
    catch (const std::invalid_argument&)
    {
        std::cerr << "Invalid accuracy: " << accStr << " - must be a positive whole number\n";
        return -1;
    }
    return accuracy;
}

cliArgs parseCLI(int argc, char* argv[])
{
    // default values
    OpenXcom::Position pos{-1, -1, -1};
    OpenXcom::Unit target{Constants::MUTON};
    AccuracyTest::FiringMode mode{AccuracyTest::Vanilla};
    bool kneeling{false};
    int accuracy{-1};
    // Manual parser
    for (int i{1}; i < argc; ++i)
    {
        std::string arg{argv[i]};

        if (arg == "--help" || arg == "-h")
        {
            printHelp(argv[0]);
            std::exit(EXIT_SUCCESS);
        }
        else if (arg == "--listunits" || arg == "-l")
        {
            printUnits();
            std::exit(EXIT_SUCCESS);
        }
        else if ((arg == "--mode" || arg == "-m") && i + 1 < argc)
        {
            std::string str(argv[++i]);
            std::transform(str.begin(), str.end(), str.begin(), ::tolower);
            if (str == "uniform")
            {
                mode = AccuracyTest::FiringMode::Uniform;
            }
            // Unnecesary as it's the default
            else if (str == "vanilla")
            {
                mode = AccuracyTest::FiringMode::Vanilla;
            }
        }
        else if ((arg == "--dist" || arg == "-d") && i + 1 < argc)
        {
            pos = parsePosition(argv[++i]);
        }
        else if ((arg == "--acc" || arg == "-a") && i + 1 < argc)
        {
            accuracy = parseAccuracy(argv[++i]);
        }
        else if ((arg == "--kneeling" || arg == "-k") && i + 1 < argc)
        {
            std::istringstream is(argv[++i]);
            // "true" and "false" will work
            is >> std::boolalpha >> kneeling;
        }
        else if ((arg == "--unit" || arg == "-u") && i + 1 < argc)
        {
            target = parseUnit(argv[++i]);
        }
    }
    return std::tuple(pos, target, mode, kneeling, accuracy);
}
} // namespace cli
