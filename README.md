Calendar
====================================================================================================

This is just a simple command-line utility to print out calendar information.


Usage
------
    calendar:  Print a calendar for a given month
    usage   :  calendar [month] [year]

    `calendar` prints the calendar for a given month. If no month is specified,
    the current month will be used. If no year is supplied, the calendar for the
    nearest month will be printed.

    v1.1.0  /  2020-05-07  /  https://github.com/hollasch/calendar


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
Steve Hollasch <steve@hollasch.net><br>
https://github.com/hollasch/calendar
