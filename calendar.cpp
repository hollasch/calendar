//======================================================================================================================
//
// calendar - print calendar for a given month or year
//
// See `help` text below for details.
//
//======================================================================================================================

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <iomanip>
#include <iostream>

using std::cout, std::cerr;


const char* version = "calendar 1.2.0 | 2023-12-01 | https://github.com/hollasch/calendar";

//--------------------------------------------------------------------------------------------------

const char help[] = R"(
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
)";


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

const int GregorianStartYear  = 1582;
const int GregorianStartMonth = 9;     // Start month of Gregorian calendar (October), base 0


//--------------------------------------------------------------------------------------------------
// Classes

struct ProgramParameters {
    int  month    { -1 };
    int  year     { -1 };
    bool startSun { false };
    const char* dowHeader;
};


//--------------------------------------------------------------------------------------------------
inline bool strEqual (const char* a, const char* b) {
    return 0 == strcmp(a,b);
}


//--------------------------------------------------------------------------------------------------
void helpExit (int exitCode) {
    cout << help << '\n' << version << '\n';
    exit(exitCode);
}


//--------------------------------------------------------------------------------------------------
std::pair<long,long> getCurrentYearMonth() {
    time_t currtime_global = time(nullptr);
    struct tm currtime_local;
    localtime_s(&currtime_local, &currtime_global);

    return { 1900 + currtime_local.tm_year, currtime_local.tm_mon - 1 };
}


//--------------------------------------------------------------------------------------------------
bool isLeapYear (int year) {
    return ((year % 4) == 0) && (((year % 100) != 0) || ((year % 400) == 0));
}


//--------------------------------------------------------------------------------------------------
int jan1DayOfWeek (int year) {
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
    // Return the number of days in the given month.

    const static int monthLengthCommon[] { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    const static int monthLengthLeap[]   { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    if (isLeapYear(year))
        return monthLengthLeap[month];

    return monthLengthCommon[month];
}


//--------------------------------------------------------------------------------------------------
int monthDayOneDOW (int year, int month) {
    // Returns the day of the week of the first of the given month and year. 0 = Sunday,
    // 6 = Saturday.

    const static int monthDay1OffsetCommon[] { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
    const static int monthDay1OffsetLeap[]   { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 };

    if (isLeapYear(year))
        return (jan1DayOfWeek(year) + monthDay1OffsetLeap[month]) % 7;

    return (jan1DayOfWeek(year) + monthDay1OffsetCommon[month]) % 7;
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
void printMonthLine (int day, int lastDay, int startDay) {
    // Print a single line of the monthly calendar (without end of line) starting at the given day.
    // Days less than one or greater than the lastDay are printed as empty spaces.

    for (int dowColumn = 0;  dowColumn < 7;  ++dowColumn, ++day) {
        if (startDay <= day && day <= lastDay)
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

    // Print four rows of three month columns.

    for (int leftMonth=0;  leftMonth < 4;  ++leftMonth) {

        cout << "\n    "   << params.dowHeader   // Print day-of-week headers
             << "        " << params.dowHeader
             << "        " << params.dowHeader;

        int day[3];      // Current month day for each column
        int lastDay[3];  // Last    month day for each column

        const bool firstGregorianYear = params.year == GregorianStartYear;

        // Initialize start and last days for each month column.

        for (int column = 0;  column < 3;  ++column) {
            int month = leftMonth + 4*column;
            day[column] = monthWeekStartDay(params.startSun, monthDayOneDOW(params.year, month)),
            lastDay[column] = monthNumDays(params.year, month);
        }

        // Print each month calendar row for all three columns, until all three are finished.

        while (day[0] <= lastDay[0] || day[1] <= lastDay[1] || day[2] <= lastDay[2]) {
            cout << '\n';
            for (int column = 0;  column < 3;  ++column) {
                int month = leftMonth + 4*column;

                if (0 < column)
                    cout << "    ";

                // Print month short name for the first row, or just blank spacing.
                if (day[column] < 2)
                    cout << monthShortNames[month] << ' ';
                else
                    cout << "    ";

                int startDay = 1;
                if (firstGregorianYear && month <= GregorianStartMonth)
                    startDay = (month == GregorianStartMonth) ? 15 : 99;

                printMonthLine(day[column], lastDay[column], startDay);
                day[column] += 7;
            }
        }

        cout << '\n';
    }
}


//--------------------------------------------------------------------------------------------------
void printMonth (const ProgramParameters& params) {
    // Prints the calendar given a month and year.

    cout << '\n' << monthNames[params.month] << ' ' << params.year << "\n\n"
         << params.dowHeader << '\n';

    int startDay = 1;
    int day = monthWeekStartDay(params.startSun, monthDayOneDOW(params.year, params.month));
    const int numDays = monthNumDays(params.year, params.month);

    // Special case for the first month of the Gregorian calendar.
    if (params.year == GregorianStartYear && params.month == GregorianStartMonth)
        startDay = 15;

    while (day <= numDays) {
        printMonthLine(day, numDays, startDay);
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

        if (arg[0] == '-' && arg[1] == '-') { // Command Options

            if (0 == _stricmp(arg, "--startSun")) {
                params.startSun = true;
            } else {
                cerr << "calendar: Unrecognized option (" << arg << ").\n";
                exit(1);
            }

        } else if (isalpha(arg[0])) { // Month Names

            for (params.month=11;  params.month >= 0;  --params.month) {
                if (0 == _strnicmp(arg, monthNames[params.month], strlen(arg)))
                    break;
            }

            if (params.month < 0) {
                cerr << "calendar: Unknown month name (" << arg << ").\n";
                exit(1);
            }

        } else { // Numeric Arguments

            int val = atoi(arg);
            if (val < 0) {
                cerr << "calendar: Negative number unexpected (" << arg << ")\n";
                exit(1);
            }

            if (val <= 12)
                params.month = val-1;
            else
                params.year = val;
        }
    }

    // Handle limits of the Gregorian calendar.

    if (0 <= params.year && params.year < GregorianStartYear) {
        cerr << "calendar: Years before the start of the Gregorian calendar (15 October 1582) are unsupported.\n";
        exit(1);
    }

    if (params.year == GregorianStartYear && (0 <= params.month && params.month < GregorianStartMonth)) {
        cerr << "calendar: Months before the start of the Gregorian calendar (15 October 1582) are unsupported.\n";
        exit(1);
    }

    params.dowHeader = dowHeaders[params.startSun ? 1 : 0];

    // If the year was not specified, print the calendar for the current year. If the specified
    // month is less than this month, assume that it's for the one in the next year.

    if (params.year >= 0)
        return params;

    long currentYear;
    long currentMonth;
    std::tie(currentYear, currentMonth) = getCurrentYearMonth();

    if (params.year < 0) {
        params.year = currentYear;

        // If months are specified, print the current or future month.
        if (params.month < 0)
            return params;
        else if (params.month < currentMonth)
            ++params.year;
    }

    return params;
}


//--------------------------------------------------------------------------------------------------
int main (int argc, char *argv[]) {
    auto params = processOptions(argc,argv);

    if (params.month < 0)
        printYear(params);
    else
        printMonth(params);

    return 0;
}
