#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <iomanip>
#include <iostream>

using std::cout, std::cerr;


const char* version = "calendar 1.2.0-alpha.5 | 2023-12-01 | https://github.com/hollasch/calendar";


//--------------------------------------------------------------------------------------------------
// Constants

const char* const monthNames[] {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};

const char* const monthShortNames[] {
    "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"
};

const char* dowHeaders[] {
    "Mo Tu We Th Fr Sa Su",
    "Su Mo Tu We Th Fr Sa"
};


//--------------------------------------------------------------------------------------------------
// Classes

struct ProgramParameters {
    int  month    { -1 };
    int  year     { -1 };
    bool startSun { false };
    const char* dowHeader;
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
           [--startSun] [month] [year]

`calendar` prints the monthly calendar for a given month. If no month is
specified, the current month will be used. If no year is supplied, the calendar
for the nearest month will be printed.

The `--startSun` option sets first day of the week as Sunday. By default, Monday
is considered the first day of the week.

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
    // This routine returns the day of the week for January 1st of the given year. 0 == Sunday,
    // 6 == Saturday.

    int dow = 1;
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

    return monthDays[isLeapYear(year) ? 1 : 0][month];
}


//--------------------------------------------------------------------------------------------------
int monthDayOne (int year, int month) {
    // Returns the day of the week of the first of the given month and year. 0 = Sunday,
    // 6 = Saturday.

    const static int monthDay1Offset [2][12] {
        {  0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 },
        {  0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 }
    };

    int leapYear = isLeapYear(year) ? 1 : 0;

    return (jan1Day(year) + monthDay1Offset[leapYear][month]) % 7;
}


//--------------------------------------------------------------------------------------------------
int monthWeekStartDay (bool startSun, int dayOne) {
    // Returns the integer day value of the first day of the week. For example, if the 1st falls on
    // a Wednesday, and the week starts on Monday, then this function returns -1. That is, Monday is
    // the -1st, Tuesday is the 0th, Wednesday is the 1st, and so on.

    if (startSun)
        return 1 - dayOne;

    return 1 - ((dayOne + 6) % 7);
}


//--------------------------------------------------------------------------------------------------
void printMonthLine (int day, int lastDay) {
    for (int dowColumn = 0;  dowColumn < 7;  ++dowColumn, ++day) {
        if (1 <= day && day <= lastDay)
            cout << std::setw(2) << day;
        else
            cout << "  ";

        if (dowColumn < 6)
            cout << ' ';
    }
}


//--------------------------------------------------------------------------------------------------
void printYear (const ProgramParameters& params) {
    // Prints the calendar for the entire given year.

    cout << "                                   --- " << params.year << " ---\n";

    for (int leftMonth=0;  leftMonth < 4;  ++leftMonth) {

        cout << "\n    "   << params.dowHeader
             << "        " << params.dowHeader
             << "        " << params.dowHeader;

        int day[3];      // Current day for each column of months
        int lastDay[3];  // Last day for each month of column

        for (int column = 0;  column < 3;  ++column) {
            int month = leftMonth + 4*column;
            day[column] = monthWeekStartDay(params.startSun, monthDayOne(params.year, month)),
            lastDay[column] = monthNumDays(params.year, month);
        }

        while (day[0] <= lastDay[0] || day[1] <= lastDay[1] || day[2] <= lastDay[2]) {
            bool firstLine = true;
            cout << '\n';
            for (int column = 0;  column < 3;  ++column) {
                int month = leftMonth + 4*column;

                if (0 < column)
                    cout << "    ";

                if (day[column] < 2)
                    cout << monthShortNames[month] << ' ';
                else
                    cout << "    ";

                printMonthLine(day[column], lastDay[column]);
                day[column] += 7;
            }
        }

        cout << '\n';
    }
}


//--------------------------------------------------------------------------------------------------
void printMonth (const ProgramParameters& params) {
    // Prints the calendar given a month and year.

    if (params.month < 0) {
        printYear(params);
        return;
    }

    cout << '\n' << monthNames[params.month] << ' ' << params.year << "\n\n"
         << params.dowHeader << '\n';

    int day = monthWeekStartDay(params.startSun, monthDayOne(params.year, params.month));
    const int numDays = monthNumDays(params.year, params.month);

    while (day <= numDays) {
        printMonthLine(day, numDays);
        cout << '\n';
        day += 7;
    }
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

            if (0 == _strnicmp(arg, "--startSun", strlen(arg))) {
                params.startSun = true;
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

    params.dowHeader = dowHeaders[params.startSun ? 1 : 0];

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

    printMonth(params);

    return 0;
}
