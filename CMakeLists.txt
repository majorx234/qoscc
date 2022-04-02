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
  src/misc.cpp
  include/misc.h
)

set(STRINGLIST_SOURCES
  src/stringlist.cpp
  include/stringlist.h
)

set(DBUFFER_SOURCES
  src/dbuffer.cpp
  include/dbuffer.h
)

set(DSPCOMMON_SOURCES
  src/dspcommon.cpp
  include/dspcommon.h
)

set(DSPALSA_SOURCES
  src/dspalsa.cpp
  include/dspalsa.h
)

set(DSPJACK_SOURCES
  src/dspjack.cpp
  include/dspjack.h
)

set(DSPMM_SOURCES
  src/dspmm.cpp
  include/dspmm.h
)

set(DSPNONE_SOURCES
  src/dspnone.cpp
  include/dspnone.h
)

set(FSLIDER_SOURCES
  src/fslider.cpp
  include/fslider.h
)

set(CONTROLLERCLASS_SOURCES
  src/controllerclass.cpp
  include/controllerclass.h
)

set(DEVICECLASS_SOURCES
  src/deviceclass.cpp
  include/deviceclass.h
)

set(TRACECLASS_SOURCES
  src/traceclass.cpp
  include/traceclass.h
)

set(SCOPECLASS_SOURCES
  src/scopeclass.cpp
  include/scopeclass.h
)

set(SCOPECONTROL_SOURCES
  src/scopecontrol.cpp
  include/scopecontrol.h
)

set(TRACECONTROL_SOURCES
  src/tracecontrol.cpp
  include/tracecontrol.h
)

set(DEVICECONTROL_SOURCES
  src/devicecontrol.cpp
  include/devicecontrol.h
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

QT5_WRAP_CPP( FSLIDER_MOCSOURCES
  ${FSLIDER_SOURCES}
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

add_library(fslider
  ${FSLIDER_SOURCES}
  ${FSLIDER_MOCSOURCES}
)
target_include_directories(fslider PRIVATE include)
target_include_directories(fslider SYSTEM
  PUBLIC 
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS})
target_link_libraries(fslider
    Qt5::Core
    Qt5::Widgets
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

###### test code ######
set(FSLIDER_TEST_SOURCES
  tests/fslider_test.cpp
)

QT5_WRAP_CPP( FSLIDER_MOCSOURCES
  ${FSLIDER_TEST_SOURCES}
)

add_executable(fslider_test 
    ${FSLIDER_TEST_SOURCES}
    ${FSLIDER_TEST_MOCSOURCES}
)
target_include_directories(fslider_test PRIVATE include)
target_include_directories(fslider_test PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(fslider_test SYSTEM
  PUBLIC 
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS})

target_link_libraries(fslider_test PUBLIC
    Qt5::Core
    Qt5::Widgets
    fslider
)