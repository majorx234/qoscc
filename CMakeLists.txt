cmake_minimum_required(VERSION 3.16)
project(qoscc
  DESCRIPTION "An oscilloscope build in QT"
  VERSION 0.0.1
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(Threads REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(ALSA REQUIRED)
find_package(Jack REQUIRED)
find_package(PkgConfig)
pkg_check_modules(FFTW3 REQUIRED fftw3)

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

set(CONTROLLERCLASS_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/controllerclass.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/controllerclass.h
)

set(DEVICECLASS_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/deviceclass.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/deviceclass.h
)

set(TRACECLASS_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/traceclass.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/traceclass.h
)

set(SCOPECLASS_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/scopeclass.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/scopeclass.h
)

set(SCOPECONTROL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/scopecontrol.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/scopecontrol.h
)

set(TRACECONTROL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/tracecontrol.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tracecontrol.h
)

set(DEVICECONTROL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/devicecontrol.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/devicecontrol.h
)

set(QOSCCWINDOW_SOURCES
    src/qosccwindow.cpp
    include/qosccwindow.h)

QT5_WRAP_UI(UISrcs 
            ui/qosccwindow.ui)

set(DATASTOREDIALOG_SOURCES
    src/datastoredialog.cpp
    include/datastoredialog.h)

QT5_WRAP_UI(DATASTOREDIALOG_UISrcs 
            ui/datastoredialog.ui)

QT5_WRAP_CPP( DATASTOREDIALOG_MOCSOURCES
  ${DATASTOREDIALOG_SOURCES}
)

QT5_WRAP_CPP( QOSCCWINDOW_MOCSOURCES
  ${QOSCCWINDOW_SOURCES}
)

QT5_WRAP_CPP( SCOPECONTROL_MOCSOURCES
  ${SCOPECONTROL_SOURCES}
)

QT5_WRAP_CPP( TRACECONTROL_MOCSOURCES
  ${TRACECONTROL_SOURCES}
)

QT5_WRAP_CPP( DEVICECONTROL_MOCSOURCES
  ${DEVICECONTROL_SOURCES}
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
target_include_directories(dbuffer PUBLIC ${FFTW3_INCLUDE_DIRS})
target_link_libraries(dbuffer
  misc 
  ${FFTW3_LIBRARIES}
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

add_library(controllerclass
  ${CONTROLLERCLASS_SOURCES}
  ${DEVICECLASS_SOURCES}
  ${TRACECLASS_SOURCES}
  ${SCOPECLASS_SOURCES}
)

target_include_directories(controllerclass PRIVATE include)
target_link_libraries(controllerclass
  stringlist
  dspcommon
  dbuffer
  dspnone
  Threads::Threads
)

add_library(datastoredialog
            ${DATASTOREDIALOG_SOURCES}
            ${DATASTOREDIALOG_MOCSOURCES}
            ${DATASTOREDIALOG_UISrcs}
            )
target_include_directories(datastoredialog PRIVATE include)
target_include_directories(datastoredialog PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(datastoredialog SYSTEM
  PUBLIC 
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS})
target_link_libraries(datastoredialog PUBLIC
    Qt5::Core
    Qt5::Widgets
)            

add_library(tracecontrol
            ${TRACECONTROL_SOURCES}
            ${TRACECONTROL_MOCSOURCES}
)
target_include_directories(tracecontrol PRIVATE include)
target_include_directories(tracecontrol PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(tracecontrol SYSTEM
  PUBLIC 
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS})
target_link_libraries(tracecontrol PUBLIC
    Qt5::Core
    Qt5::Widgets
)

add_library(devicecontrol
            ${DEVICECONTROL_SOURCES}
            ${DEVICECONTROL_MOCSOURCES}
)
target_include_directories(devicecontrol PRIVATE include)
target_include_directories(devicecontrol PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(devicecontrol SYSTEM
  PUBLIC 
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS})
target_link_libraries(devicecontrol PUBLIC
    Qt5::Core
    Qt5::Widgets
)

add_library(scopecontrol
            ${SCOPECONTROL_SOURCES}
            ${SCOPECONTROL_MOCSOURCES}
)
target_include_directories(scopecontrol PRIVATE include)
target_include_directories(scopecontrol PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(scopecontrol SYSTEM
  PUBLIC 
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS})
target_link_libraries(scopecontrol PUBLIC
    Qt5::Core
    Qt5::Widgets
    controllerclass
    datastoredialog
)

add_library(qosccwindow
            ${QOSCCWINDOW_SOURCES}
            ${QOSCCWINDOW_MOCSOURCES}
            ${UISrcs}
            )

target_include_directories(qosccwindow PRIVATE include)
target_include_directories(qosccwindow PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(qosccwindow SYSTEM
  PUBLIC 
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS})
target_link_libraries(qosccwindow PUBLIC
    ${JACK_LIBRARIES}
    Qt5::Core
    Qt5::Widgets
    controllerclass
    scopecontrol
    devicecontrol
    tracecontrol
)

add_executable(qoscc_test
  src/main.cpp
)

target_include_directories(qoscc_test PRIVATE include)
target_link_libraries(qoscc_test
  qosccwindow 
)
