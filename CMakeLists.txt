project(qsopcast)

set(VERSION_MAJOR "0")
set(VERSION_MINOR "4")
set(VERSION_PATCH "86")
set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

cmake_minimum_required(VERSION 2.6.0)

find_package(Qt4 REQUIRED)

# enable all compiler warnings
add_definitions(-Wall)

add_subdirectory(src)
