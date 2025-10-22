#include "cli.h"
#include "Constants.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <stdlib.h>
#include <string>
#include <vector>

// TODO: i REALLY need some kind of gui but i guess this cli works
namespace cli
{
std::string TrimWhitespace(const std::string& str)
{
    size_t first = str.find_first_not_of(" \t");
    size_t last = str.find_last_not_of(" \t");
    if (first == std::string::npos || last == std::string::npos)
        return "";
    return str.substr(first, last - first + 1);
}

void PrintHelp(const std::string& progName)
{
    std::cout << "Usage: " << progName << " (--dist | -d) <'x,y,z'> [(--acc | -a) <integer>]\n\n"
              << "Options:\n"
              << "  --help,       -h                      | Show this help message\n"
              << "  --listunits,  -l                      | Show all units that can be used.\n"
              << "  --dist,       -d <'x,y,z'>            | 3D distance between the shooter and target in tiles as x,y,z (required)\n"
              << "                                        | Omitting the second or third coordinate values will set them as 0.\n"
              << "  --unit,       -u <name>               | Name of the unit you are aiming at. (default: Muton)\n"
              << "  --kneel,      -k <true|false>         | Is the unit you are aiming at kneeling or not. (default: false)\n"
              << "  --mode,       -m <vanilla|uniform>    | Chooses the spread model that you wish to test. (default: vanilla)\n"
              << "  --acc,        -a <N>                  | Accuracy value in whole percents as seen on the crosshair. (default: omitted)\n"
              << "                                        | If omitted, runs a full accuracy sweep of [0%, 110%] and writes to a file named output.csv\n"
              << std::flush;
}

void ListUnits()
{
    std::cout << "List of all available units:\n"
              << "celatid,\nchryssalid (= muton),\ncivilian (= muton),\ncyberdisc,\nethereal,\nfloater (= muton),\nhovertank,\nmuton,\nreaper (= sectopod),\nsectoid,\nsectopod,\nsilacoid,\nsoldier,\ntank,\n"
              << std::flush;
}

OpenXcom::Unit ParseUnit(std::string Str)
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

OpenXcom::Position ParsePosition(const std::string& coordStr)
{
    std::stringstream ss(coordStr);
    std::vector<int> coords{0, 0, 0};
    std::string coord;
    for (auto& val : coords)
    {
        try
        {
            val = std::getline(ss, coord, ',') ? std::stoi(coord) : 0;
        }
        catch (const std::invalid_argument& err)
        {
            std::cerr << "Invalid coordinate value: " << coord << "\n";
            return OpenXcom::Position{-1, -1, -1};
        }
    }
    // while (std::getline(ss, coord, ','))
    // {
    //     try
    //     {
    //         coords.push_back(std::stoi(coord));
    //     }
    //     catch (const std::invalid_argument& err)
    //     {
    //         std::cerr << "Invalid coordinate value: " << coord << "\n";
    //         return OpenXcom::Position{-1, -1, -1};
    //     }
    // }
    return OpenXcom::Position{coords[0] * Constants::TILE_WIDTH,
                              coords[1] * Constants::TILE_WIDTH,
                              coords[2] * Constants::TILE_HEIGHT};
}

int ParseAccuracy(const std::string& accStr)
{
    int accuracy{};
    try
    {
        accuracy = std::stoi(accStr);
        if (accuracy < 0)
        {
            throw std::invalid_argument("Negative accuracy value.");
        }
    }
    catch (const std::invalid_argument& err)
    {
        std::cerr << "Invalid accuracy: " << accStr << " - must be a positive whole number\nProceeding with full accuracy sweep.\n";
        return -1;
    }
    return accuracy;
}

cliArgs ParseCLI(int argc, char* argv[])
{
    // default values
    OpenXcom::Position pos{-1, -1, -1};
    OpenXcom::Unit target{Constants::MUTON};
    AccuracyTest::FiringMode mode{AccuracyTest::FiringMode::Vanilla};
    bool kneeling{false};
    int accuracy{-1};
    // Manual parser
    for (int i{1}; i < argc; ++i)
    {
        std::string arg{argv[i]};

        if (arg == "--help" || arg == "-h")
        {
            PrintHelp(argv[0]);
            std::exit(EXIT_SUCCESS);
        }
        else if (arg == "--listunits" || arg == "-l")
        {
            ListUnits();
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
            pos = ParsePosition(argv[++i]);
        }
        else if ((arg == "--acc" || arg == "-a") && i + 1 < argc)
        {
            accuracy = ParseAccuracy(argv[++i]);
        }
        else if ((arg == "--kneel" || arg == "-k") && i + 1 < argc)
        {
            std::istringstream is(argv[++i]);
            // "true" and "false" will work
            is >> std::boolalpha >> kneeling;
        }
        else if ((arg == "--unit" || arg == "-u") && i + 1 < argc)
        {
            target = ParseUnit(argv[++i]);
        }
    }
    return std::tuple(pos, target, mode, kneeling, accuracy);
}
} // namespace cli
