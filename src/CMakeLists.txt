# CMake File for `calendar`

cmake_minimum_required (VERSION 3.0)

project (calendar)

set (calendar_VERSION_MAJOR 1)
set (calendar_VERSION_MINOR 0)
set (calendar_VERSION_PATCH 1)

configure_file (
    "${PROJECT_SOURCE_DIR}/calendarVersion.h.in"
    "${PROJECT_BINARY_DIR}/calendarVersion.h"
)

include_directories ("${PROJECT_BINARY_DIR}")

add_executable (calendar calendar.cpp)
