#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <iomanip>
#include <iostream>

using std::cout, std::cerr;


const char* version = "calendar 1.2.0-alpha.3 | 2023-11-27 | https://github.com/hollasch/calendar";


//--------------------------------------------------------------------------------------------------
// Classes

struct ProgramParameters {
    int  month    { -1 };
    int  year     { -1 };
    bool startSun { false };
};


//--------------------------------------------------------------------------------------------------
// Constants

const char* const monthNames[] {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};

const char* const monthShortNames[] {
    "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"
};

const char* days[] {
    "xx xx xx xx xx xx  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31"
};

//--------------------------------------------------------------------------------------------------
// Utility Functions

inline bool strEqual (const char* a, const char* b) {
    return 0 == strcmp(a,b);
}


//--------------------------------------------------------------------------------------------------
// Help

const char help[] = R"(
calendar:  Print a calendar for a given month
usage   :  calendar [-h|/?|--help] [-v|--version]
           [--weekStart <Mo[n[day]]|<Su[n[day]]>
           [month] [year]

`calendar` prints the monthly calendar for a given month. If no month is
specified, the current month will be used. If no year is supplied, the calendar
for the nearest month will be printed.

The `--weekStart` option sets the specified day as the first day of the week.
The choices are "mo", "mon", "monday", "su", "sun", or "sunday" (case
insensitive). By default, Monday is the first day of the week.

A month is recognized as starting with a letter, or if it is a number between
1 and 12 with only one or two digits.
)";


void helpExit(int exitCode) {
    cout << help << '\n' << version << '\n';
    exit(exitCode);
}


//--------------------------------------------------------------------------------------------------
bool isLeapYear(int year) {
    return ((year % 4) == 0) && (((year % 100) != 0) || ((year % 400) == 0));
}


//--------------------------------------------------------------------------------------------------
int jan1Day (int year) {
    // This routine returns the day of the week for January 1st of the given year.

    int dow = 0;
    int yearFrac = (year-1) % 400;

    while (yearFrac >= 100) {
        dow += 5;
        yearFrac -= 100;
    }

    dow += yearFrac + int(yearFrac/4);

    return dow % 7;
}


//--------------------------------------------------------------------------------------------------
int monthNumDays (int year, int month) {
    // This procedure computes the information about the given month.

    const static int monthDays [2][12] = {
        { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
        { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
    };

    return monthDays[isLeapYear(year) ? 0 : 1][month];
}


//--------------------------------------------------------------------------------------------------
int monthDayOne (int year, int month) {
    const static int monthDay1 [2][12] {
        {  0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 },
        {  0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 }
    };

    return jan1Day(year) + monthDay1[isLeapYear(year) ? 0 : 1][month];
}


//--------------------------------------------------------------------------------------------------
void printCal (int year) {
    // Prints the calendar for the entire given year.
    cout << "                                   --- " << year << " ---\n\n";

    for (int leftMonth=0;  leftMonth < 4;  ++leftMonth) {
        cout << "\n    Su Mo Tu We Th Fr Sa        Su Mo Tu We Th Fr Sa        Su Mo Tu We Th Fr Sa\n";

        bool monthFinished[3] { false, false, false };
        int  monthsFinished = 0;
        int  monthLine = 0;

        while (monthsFinished < 3) {
            for (int relMonth=0;  relMonth < 3;  ++relMonth) {
                if (relMonth > 0)
                    cout << "    ";

                if (monthLine == 0) {
                    cout << monthShortNames[leftMonth + relMonth] << ' ';
                } else {
                    cout << "    ";
                }
                cout << "xx xx xx xx xx xx xx";
            }

            ++monthLine;
            if (monthLine > 5)
                monthsFinished = 3;

            cout << '\n';
        }
    }
}


//--------------------------------------------------------------------------------------------------
void printMonth (const ProgramParameters& params) {
    // Prints the calendar given a month and year.

    if (params.month < 0) {
        printCal(params.year);
        return;
    }

    cout << '\n' << monthNames[params.month] << ", " << params.year << "\n\n";
    cout << "Mo Tu We Th Fr Sa Su\n";

    int calSlot = 0;
    int day = 0;

    const int dayOne = monthDayOne(params.year, params.month);
    const int numDays = monthNumDays(params.year, params.month);

    cout << "Number of days: " << numDays << '\n';
    cout << "Day One: " << dayOne << " -> " << (dayOne % 7) << "\n\n";

    do {
        if (calSlot++ < dayOne) {
            cout << "   ";
            continue;
        }

        cout << std::setw(2) << ++day << ' ';

        if ((calSlot % 7) == 0) cout << '\n';

    } while (day < numDays);

    if ((calSlot % 7) != 0) cout << '\n';
}


//--------------------------------------------------------------------------------------------------
ProgramParameters processOptions (int argc, char *argv[]) {
    ProgramParameters params;

    // This routine parses the command-line arguments and determines the requested period to print.

    for (int argi=1;  argi < argc;  ++argi) {

        char *arg = argv[argi];

        if (strEqual(arg, "/?") || strEqual(arg, "-h") || strEqual(arg, "--help")) {
            helpExit(0);
        }

        if (strEqual(arg, "-v") || strEqual(arg, "--version")) {
            cout << version << '\n';
            exit(0);
        }

        // If the first char is an alpha, then it must be a month, else we need more info.

        if (arg[0] == '-' && arg[1] == '-') {

            if (0 == _strnicmp(arg, "--weekstart", strlen(arg))) {
                if (argc <= ++argi) {
                    cerr << "calendar: Missing expected argument to --weekStart option.\n";
                    exit(1);
                }
                arg = argv[argi];

                params.startSun = 0 == _stricmp(arg, "su") ||
                                  0 == _stricmp(arg, "sun") ||
                                  0 == _stricmp(arg, "sunday");

                if (0 == _stricmp(arg, "su") || 0 == _stricmp(arg, "sun") || 0 == _stricmp(arg, "sunday")) {
                    params.startSun = true;
                } else if (0 != _stricmp(arg, "mo") && 0 != _stricmp(arg, "mon") && 0 != _stricmp(arg, "monday")) {
                    cerr << "calendar: Unrecognized --weekstart option (" << arg << ").\n";
                    exit(1);
                }
            } else {
                cerr << "calendar: Unrecognized option (" << arg << ").\n";
                exit(1);
            }

        } else if (isalpha(arg[0])) {

            auto mlen = strlen(arg);
            for (params.month=11;  params.month >= 0;  --params.month) {
                if (0 == _strnicmp(arg, monthNames[params.month], mlen))
                    break;
            }

            if (params.month < 0) {
                cerr << "calendar: Unknown month name (" << arg << ").\n";
                exit(1);
            }

        } else {

            int val = atoi(arg);
            if (val < 0) {
                cerr << "calendar: Negative number unexpected (" << arg << ")\n";
                exit(1);
            }

            // The numeric argument is a year if it's not in [1,12], or if it's greater than two
            // digits (and leading zero).

            if ((val < 1) || (12 < val) || (strlen(arg) > 2))
                params.year = val;
            else
                params.month = val-1;
        }
    }

    // If the year was not specified, print the calendar for the current year. If the specified
    // month is less than this month, assume that it's for the one in the next year.

    if (params.year >= 0)
        return params;

    time_t currtime_global = time(nullptr);
    struct tm currtime_local;
    localtime_s(&currtime_local, &currtime_global);

    if (params.year < 0) {
        params.year = 1900 + currtime_local.tm_year;

        if (params.month < 0)
            return params;
        else if (params.month < currtime_local.tm_mon)
            ++params.year;
    }

    return params;
}


//--------------------------------------------------------------------------------------------------
int main (int argc, char *argv[]) {
    auto params = processOptions(argc,argv);

    cout << "params.startSun = " << (params.startSun ? "true\n" : "false\n");
    cout << "params.month    = " << params.month << '\n';
    cout << "params.year     = " << params.year << '\n';

    printMonth(params);

    return 0;
}
