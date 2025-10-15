# OpenXcom Accuracy to Hitchance Converter
Convert the not-so-accurate accuracy value in OpenXcom to an accurate hit chance percentage so you can make better decisions for which shots to take.

Uses a command-line interface that allows you to pass in the required information to turn the not-so-accurate accuracy number on a shot (or your crosshair with UFOExtender accuracy) to an accurate hit chance percentage.
The output will be rounded to a single decimal position.

There are 2 versions of the program: a C++ version and a Python version. The difference in the versions is that C++ requires to be compiled, but will be faster than python.

## Set-up

C++: You will have to compile the source files in .\src\ for your computer/architecture (which I don't know enough of to give you any help).

Python: You will need to install Python and have it be added to your path environment (which I believe the python installer asks if you want to do).

## Usage
Navigate to the folder with the executable file and open a command prompt there (Shift+RMB in the folder will give the "Open Command Prompt here" option).
Alternatively navigate to the folder with a command prompt (using ls and cd commands) and run:

C++: `.\HitChanceCalc.exe <Option> <Value>`

Python: `python  .\HitChanceCalc.py <Option> <Value>`

Options list:
```
--help,       -h                      | Shows the list of options and what they mean.
--listunits,  -l                      | Shows all units that can be shot at.
--dist,       -d <x,y,z>              | 3D distance between the shooter and target in tiles as x,y,z (required)
--unit,       -u <name>               | Name of the unit you are aiming at. (default: Muton)
--kneel,      -k <true|false>         | Is the unit you are aiming at kneeling or not. (default: false)
--mode,       -m <vanilla|uniform>    | Chooses the spread model that you wish to test. (default: vanilla)
--acc,        -a <N>                  | Accuracy value in whole percents as seen on the crosshair. (default: omitted).
                                      | If omitted, runs a full accuracy sweep of [0%, 110%] and writes to an output file named output.csv.
```
The order of options does not matter, however --help and --listunits will take precendence to other options.

For some units the result will give you an incorrect displayed unit name (e.g. muton instead of floater). This is because they have the same hitbox size and should be therefore have the same (or at least very similar) hit chance.

For --dist you must put the 3 coordinate values inside quotes to group them together as one value. (see examples below)

## Examples

.\HitChanceCalc.exe -d "14, 2, 1" -a 73
> The hit chance for shooting a(n) Muton (14, 2, 1) tiles away with an accuracy of 73% is: 76.7%

.\HitChanceCalc.exe -m Uniform -u Chryssalid  -a 46 -d "4, 6, 0"
> The hit chance for shooting a(n) Muton (4, 6, 0) tiles away with an accuracy of 46% is: 61.0%

.\HitChanceCalc.exe -m Uniform -u Soldier -d "7, 3, 0" -a 37 -k true
>The hit chance for shooting a(n) XCOM Soldier (7, 3, 0) tiles away with an accuracy of 37% is: 51.7%

.\HitChanceCalc.exe -m Vanilla  --unit Cyberdisc --dist "21, 13, 2" -k false
(Note: Output will be in "output.csv")
> Cyberdisc at (21, 13, 2) Done!
