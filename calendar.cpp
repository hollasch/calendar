#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>


const auto version = "v1.1.1  2020-05-21  https://github.com/hollasch/calendar";

struct ProgramParameters {
    int month {-1};
    int year {-1};
};

const char * const monthnames[] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};

const char days[] =
    " 1  2  3  4  5  6  7  8  9 10 "
    "11 12 13 14 15 16 17 18 19 20 "
    "21 22 23 24 25 26 27 28 29 30 "
    "31";


inline void print (const char* string) {
    fputs(string, stdout);
}

inline bool streq (const char* a, const char* b) {
    return 0 == strcmp(a,b);
}


const char usage[] = R"(
calendar:  Print a calendar for a given month
usage   :  calendar [-h|/?|--help] [--version] [month] [year]

`calendar` prints the calendar for a given month. If no month is specified,
the current month will be used. If no year is supplied, the calendar for the
nearest month will be printed.
)";


void UsageExit() {
    fprintf(stderr, usage);
    exit(1);
}


int Jan1Day (int year) {
    // This routine returns the day of the week for January 1st of the given year.

    int dow = 0;
    int yearfrac = (year-1) % 400;

    while (yearfrac >= 100) {
        dow += 5;
        yearfrac -= 100;
    }

    dow += yearfrac + int(yearfrac/4);

    return dow % 7;
}



void MonthInfo (const int month, const int year, int &dayone, int &numdays) {
    // This procedure computes the information about the given month.

    const static int dayofmonth1 [2][12] = {
        {  0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 },
        {  0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 }
    };

    const static int monthdays [2][12] = {
        { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
        { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
    };

    // Leapyear if divisible by four and not divisible by 100, unless also divisible by 400.

    int leapyear = ((year%4) == 0) && (((year%100) != 0) || ((year%400) == 0));

    numdays = monthdays [leapyear] [month];

    // Day one of the specified month is the day of January 1st of that year, plus the day of the
    // prior months, all modulo 7 (days in a week).

    dayone = (Jan1Day(year) + dayofmonth1[leapyear][month]) % 7;
}



void PrintCal (int year) {
    // Prints the calendar for the entire given year.
    printf("%35c--- %d ---\n\n", ' ', year);
    printf("    Mo Tu We Th Fr  Sa Su      Mo Tu We Th Fr  Sa Su      Mo Tu We Th Fr  Sa Su\n");
}


void PrintMonth (const ProgramParameters& params) {
    // Prints the calendar given a month and year.

    if (params.month < 0) {
        PrintCal(params.year);
        return;
    }

    printf("\n%s, %u\n\n", monthnames[params.month], params.year);
    print("Mo Tu We Th Fr Sa Su\n");

    int week=0, calslot=0, day=0;
    int dayone, numdays;

    MonthInfo(params.month, params.year, dayone, numdays);

    do {
        if (calslot++ < dayone) {
            print("   ");
            continue;
        }

        printf("%2d ", ++day);

        if ((calslot % 7) == 0) print("\n");

    } while (day < numdays);

    if ((calslot % 7) != 0) print("\n");
}


ProgramParameters ProcessOptions (int argc, char *argv[]) {
    ProgramParameters params;

    // This routine parses the command-line arguments and determines the requested period to print.

    for (int argi=1;  argi < argc;  ++argi) {

        char *arg = argv[argi];

        if (streq(arg, "/?") || streq(arg, "-h") || streq(arg, "--help")) {
            puts(usage);
            puts(version);
            exit(0);
        }

        if (streq(arg, "--version")) {
            printf("calendar  %s\n", version);
            exit(0);
        }

        // If the first char is an alpha, then it must be a month, else we need more info.

        if (isalpha(arg[0])) {

            auto mlen = strlen(arg);
            for (params.month=11;  params.month >= 0;  --params.month) {
                if (0 == _strnicmp(arg, monthnames[params.month], mlen))
                    break;
            }

            if (params.month < 0) {
                fprintf(stderr, "calendar:  Unknown month name (%s)\n", arg);
                UsageExit();
            }

        } else {

            int val = atoi(arg);
            if (val < 0) {
                fprintf(stderr, "calendar:  Negative number unexpected (%d)\n", val);
                UsageExit();
            }

            // The numeric argument is a year if it's not in [1,12], or if it's greater than two
            // digits (and leading zero).

            if ((val < 1) || (12 < val) || (strlen(arg) > 2))
                params.year = val;
            else
                params.month = val-1;
        }
    }

    // If the year was not specified, print the calendar for the current year. If the month is less
    // than this month, assume that it's for the one in the next year.

    if (params.year < 0) {
        time_t currtime_global = time(nullptr);
        struct tm currtime_local;
        localtime_s(&currtime_local, &currtime_global);

        params.year = 1900 + currtime_local.tm_year;

        if (params.month < 0)
            params.month = currtime_local.tm_mon;
        else if (params.month < currtime_local.tm_mon)
            ++params.year;
    }

    // Check to make sure that the month was specified.

    if (params.month < 0) {
        fprintf(stderr, "calendar: No month specified.\n");
        UsageExit();
    }

    return params;
}


int main (int argc, char *argv[]) {
    PrintMonth(ProcessOptions(argc,argv));
    return 0;
}
