Change Log -- calendar
====================================================================================================

# v2.0.0  (2024-02-05)

### Change
  - Made the annual calendar print row major. That is, months go left-to-right, then top-to-bottom.


----------------------------------------------------------------------------------------------------
# v1.2.0  (2023-12-01)

### Fix
  - Fix two error-reporting cases

### Change
  - Dates before the start of the Gregorian calendar (1582 October 15) yield an error.

### New
  - Proper handling of monthly and annual calendars around the start of the Gregorian calendar.
  - Add option to print an entire year when the month is not specified.
  - New `--startSun` to print calendars using Sunday as the first column (defaulting to Monday).


----------------------------------------------------------------------------------------------------
# v1.1.2  (2021-04-20)

### Change
  - Updated version format
  - Updated help output
  - Switch to using IO streams


----------------------------------------------------------------------------------------------------
# v1.1.1  (2021-04-20)

### Fix
  - Fix build warnings
  - General code cleanup and reformatting

### New
  - Updated to C++20


----------------------------------------------------------------------------------------------------
# v1.1.0  (2020-05-07)

### New
  - Converted project to build with CMake


----------------------------------------------------------------------------------------------------
# v1.0.0  (2018-06-24)

### New
  - First public release
