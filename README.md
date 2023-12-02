Calendar
====================================================================================================

A simple command-line utility to print out month or annual calendars.


Usage
------
    calendar:  Print calendar for a given month or year
    usage   :  calendar [-h|/?|--help] [-v|--version]
               [--startSun] [month] [year]

    `calendar` prints the calendar for a given month or year. If no month is
    specified, the current annual calendar is printed. If a month is specified
    without year, then the nearest current or future month is printed.

    This tool only supports the Gregorian calendar, so years before 1582 or months
    before October 1582 are not supported.

    A month is specified using either its English name, or as a number from 1 to 12.
    A year is specified as a number greater than 12, though years before 1582 are
    unsupported.

    The `--startSun` option sets first day of the week as Sunday. By default, Monday
    is considered the first day of the week.

    calendar 1.2.0 | 2023-12-01 | https://github.com/hollasch/calendar


Building
----------
This project uses the CMake build tool. CMake is a meta-build system that locates and uses your
local development tools to build the project if possible.

To build, first install [CMake][https://cmake.org/]. Then go to the project root directory and run
the following command:

    cmake -B build

This will locate your installed development tools and configure your project build in the `build/`
directory. After that, whenever you want a new build, run this command:

    cmake --build build

This will build a debug version of the project, located in `build/Debug/`. To build a release
version, run

    cmake --build build --config release

You can find the built release executable in `build/Release/`.


----------------------------------------------------------------------------------------------------
Steve Hollasch &lt;steve@hollasch.net&gt;<br>
https://github.com/hollasch/calendar
