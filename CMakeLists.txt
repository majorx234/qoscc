cmake_minimum_required(VERSION 3.16)
project(qoscc
  DESCRIPTION "An oscilloscope build in QT"
  VERSION 0.0.1
  LANGUAGES CXX)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(Threads REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(ALSA REQUIRED)
find_package(Jack REQUIRED)

set(MISC_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/misc.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/misc.h
)

set(DBUFFER_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/dbuffer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/dbuffer.h
)

add_library(misc
  ${MISC_SOURCES}
)

target_include_directories(misc PRIVATE include)

add_library(dbuffer
  ${DBUFFER_SOURCES}
)

target_include_directories(dbuffer PRIVATE include)
target_link_libraries(dbuffer
  misc 
)
