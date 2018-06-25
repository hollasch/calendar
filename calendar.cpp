#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>


static void GetPeriod (int argc, char *argv[], int &month, int &year);
static void MonthInfo (const int mon, const int yr, int &dayone, int &numdays);
static void PrintCal (int year);
static void PrintMonth (int month, int year);
static int  Jan1Day (int year);

static inline void print(char*string) { fputs (string, stdout); }


const char usage[] = R"(
calendar:  Print a calendar for a given month
usage   :  calendar [month] [year]

`calendar` prints the calendar for a given month. If no month is specified,
the current month will be used. If no year is supplied, the calendar for the
nearest month will be printed.

v1.0.0  /  2018-06-24  /  https://github.com/hollasch/calendar
)";


const static char * const monthnames[] =
{   "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};

const static char days[] =
" 1  2  3  4  5  6  7  8  9 10 "
"11 12 13 14 15 16 17 18 19 20 "
"21 22 23 24 25 26 27 28 29 30 "
"31";



//------------------------------------------------------------------------------
// Main Routine
//------------------------------------------------------------------------------

int main (int argc, char *argv[])
{
    int month, year;

    GetPeriod (argc, argv, month, year);

    PrintMonth (month, year);

    return 0;
}



//------------------------------------------------------------------------------
// This routine parses the command-line arguments and determines the requested
// period to print.
//------------------------------------------------------------------------------

static void GetPeriod (
    int   argc,
    char *argv[],
    int  &month,
    int  &year)
{
    time_t currtime_t;
    time (&currtime_t);
    struct tm *currtime = localtime (&currtime_t);

    month = -1;
    year  = -1;

    for (int argi=1;  argi < argc;  ++argi)
    {
        char *arg = argv[argi];

        // If the first char is an alpha, then it must be a month, else we
        // need more info.

        if (isalpha(arg[0]))
        {
            int mlen = strlen (arg);
            for (month=11;  month >= 0;  --month)
            {   if (0 == _strnicmp (arg, monthnames[month], mlen))
                    break;
            }

            if (month < 0)
            {   fprintf (stderr, "calendar:  Unknown month name (%s)\n", arg);
                fprintf (stderr, usage);
                exit (1);
            }
        }
        else
        {
            int val = atoi (arg);
            if (val < 0)
            {   fprintf (stderr,
                    "calendar:  Negative number unexpected (%d)\n", val);
                fprintf (stderr, usage);
                exit (1);
            }

            // The numeric argument is a year if it's not in [1,12], or if
            // it's greater than two digits (and leading zero).

            if ((val < 1) || (12 < val) || (strlen(arg) > 2))
                year = val;
            else
                month = val-1;
        }
    }

    // If the year was not specified, print the calendar for the current year.
    // If the month is less than this month, assume that it's for the one in the
    // next year.

    if (year < 0)
    {
        year = 1900 + currtime->tm_year;

        if (month < 0)
            month = currtime->tm_mon;
        else if (month < currtime->tm_mon)
            ++year;
    }

    // Check to make sure that the month was specified.

    if (month < 0)
    {
        fprintf (stderr, "calendar: No month specified.\n");
        fprintf (stderr, usage);
        exit (1);
    }
}



//------------------------------------------------------------------------------
// Prints the calendar given a month and year.
//------------------------------------------------------------------------------

void PrintMonth (int month, int year)
{
    if (month < 0)
    {   PrintCal (year);
        return;
    }

    printf ("\n%s, %u\n\n", monthnames[month], year);
    print  ("Mo Tu We Th Fr Sa Su\n");

    int week=0, calslot=0, day=0;
    int dayone, numdays;

    MonthInfo (month, year, dayone, numdays);

    do
    {   if (calslot++ < dayone)
        {   print ("   ");
            continue;
        }

        printf ("%2d ", ++day);

        if ((calslot % 7) == 0) print ("\n");

    } while (day < numdays);

    if ((calslot % 7) != 0) print ("\n");
}



//------------------------------------------------------------------------------
// This procedure computes the information about the given month.
//------------------------------------------------------------------------------

void MonthInfo (
    const int  month,
    const int  year,
          int &dayone,
          int &numdays)
{
    const static int dayofmonth1 [2][12] =
    {   {  0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 },
        {  0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 }
    };

    const static int monthdays [2][12] =
    {   { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
        { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
    };

    // Leapyear if divisible by four and not divisible by 100, unless also
    // divisible by 400.

    int leapyear = ((year%4) == 0) && (((year%100) != 0) || ((year%400) == 0));

    numdays = monthdays [leapyear] [month];

    // Day one of the specified month is the day of January 1st of that year,
    // plus the day of the prior months, all modulo 7 (days in a week).

    dayone = (Jan1Day(year) + dayofmonth1[leapyear][month]) % 7;
}



//------------------------------------------------------------------------------
// This routine returns the day of the week for January 1st of the given year.
//------------------------------------------------------------------------------

static int Jan1Day (int year)
{
    int dow = 0;
    int yearfrac = (year-1) % 400;

    while (yearfrac >= 100)
    {
        dow += 5;
        yearfrac -= 100;
    }

    dow += yearfrac + int(yearfrac/4);

    return dow % 7;
}



//------------------------------------------------------------------------------
// Prints the calendar for the entire given year.
//------------------------------------------------------------------------------

void PrintCal (int year)
{
    printf ("%35c--- %d ---\n\n", ' ', year);
    printf ("    Mo Tu We Th Fr  Sa Su      Mo Tu We Th Fr  Sa Su      Mo Tu We Th Fr  Sa Su\n");
}
