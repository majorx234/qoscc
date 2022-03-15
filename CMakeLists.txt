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

set(STRINGLIST_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/stringlist.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/stringlist.h
)

set(DBUFFER_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/dbuffer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/dbuffer.h
)

set(DSPCOMMON_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/dspcommon.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/dspcommon.h
)

set(DSPALSA_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/dspalsa.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/dspalsa.h
)

set(DSPJACK_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/dspjack.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/dspjack.h
)

set(DSPMM_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/dspmm.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/dspmm.h
)

set(DSPNONE_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/dspnone.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/dspnone.h
)

set(QOSCWINDOW_SOURCES
    src/qoscwindow.cpp
    include/qoscwindow.h)

QT5_WRAP_UI(UISrcs 
            ui/qoscwindow.ui)

QT5_WRAP_CPP( QOSCWINDOW_MOCSOURCES
  ${QOSCWINDOW_SOURCES}
)

add_library(misc
  ${MISC_SOURCES}
)

target_include_directories(misc PRIVATE include)

add_library(stringlist
  ${STRINGLIST_SOURCES}
)

target_include_directories(stringlist PRIVATE include)

add_library(dbuffer
  ${DBUFFER_SOURCES}
)

target_include_directories(dbuffer PRIVATE include)
target_link_libraries(dbuffer
  misc 
)

add_library(dspcommon
  ${DSPCOMMON_SOURCES}
)

target_include_directories(dspcommon PRIVATE include)
target_link_libraries(dspcommon
  dbuffer
  stringlist
  Threads::Threads
)

add_library(dspalsa
  ${DSPALSA_SOURCES}
)

target_include_directories(dspalsa PRIVATE include)
target_include_directories(dspalsa PUBLIC ${ALSA_INCLUDE_DIR})
target_link_libraries(dspalsa
  ${ALSA_LIBRARIES}
  dspcommon
  dbuffer
)

add_library(dspjack
  ${DSPJACK_SOURCES}
)

target_include_directories(dspjack PRIVATE include)
target_include_directories(dspjack PUBLIC ${JACK_INCLUDE_DIRS})
target_link_libraries(dspjack
  ${JACK_LIBRARIES}
  dspcommon
  dbuffer
)

add_library(dspmm
  ${DSPMM_SOURCES}
)

target_include_directories(dspmm PRIVATE include)
target_link_libraries(dspmm
  dspcommon
  dbuffer
)

add_library(dspnone
  ${DSPNONE_SOURCES}
)

target_include_directories(dspnone PRIVATE include)
target_link_libraries(dspnone
  dspcommon
  dbuffer
)

add_library(qoscwindow
            ${QOSCWINDOW_SOURCES}
            ${QOSCWINDOW_MOCSOURCES}
            ${UISrcs}
            )

target_include_directories(qoscwindow PRIVATE include)
target_include_directories(qoscwindow PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(qoscwindow SYSTEM
  PUBLIC 
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS})
target_link_libraries(qoscwindow PUBLIC 
    ${JACK_LIBRARIES}
    Qt5::Core
    Qt5::Widgets)

add_executable(qosc_test
  src/main.cpp
)

target_include_directories(qosc_test PRIVATE include)
target_link_libraries(qosc_test
  qoscwindow 
)
