IF (CMAKE_VERSION VERSION_LESS 3.0)
  PROJECT(ModbusThings CXX)
  SET(PROJECT_VERSION_MAJOR "${LIB_MAJOR_VERSION}")
  SET(PROJECT_VERSION_MINOR "${LIB_MINOR_VERSION}")
  SET(PROJECT_VERSION_PATCH "${LIB_PATCH_VERSION}")
  SET(PROJECT_VERSION "${LIB_VERSION_STRING}")
ELSE ()
  CMAKE_POLICY(SET CMP0048 NEW)
  PROJECT(ModbusThings VERSION "${LIB_VERSION_STRING}" LANGUAGES CXX)
ENDIF ()

IF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  CMAKE_MINIMUM_REQUIRED(VERSION 3.0...3.20)
ELSE ()
  CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
ENDIF ()

if(NOT WIN32)
  string(ASCII 27 Esc)
  set(ColourReset "${Esc}[m")
  set(ColourBold  "${Esc}[1m")
  set(Red         "${Esc}[31m")
  set(Green       "${Esc}[32m")
  set(Yellow      "${Esc}[33m")
  set(Blue        "${Esc}[34m")
  set(Magenta     "${Esc}[35m")
  set(Cyan        "${Esc}[36m")
  set(White       "${Esc}[37m")
  set(BoldRed     "${Esc}[1;31m")
  set(BoldGreen   "${Esc}[1;32m")
  set(BoldYellow  "${Esc}[1;33m")
  set(BoldBlue    "${Esc}[1;34m")
  set(BoldMagenta "${Esc}[1;35m")
  set(BoldCyan    "${Esc}[1;36m")
  set(BoldWhite   "${Esc}[1;37m")
endif()


SET(TD_MEM_CHECK FALSE)


SET(TD_COMMUNITY_DIR ${PROJECT_SOURCE_DIR})
MESSAGE(STATUS "Community directory: " ${TD_COMMUNITY_DIR})


INCLUDE(cmake/platform.inc)

IF (TD_WINDOWS OR TD_DARWIN)
  SET(TD_SOMODE_STATIC TRUE)
ENDIF ()

INCLUDE(cmake/define.inc)
INCLUDE(cmake/env.inc)
INCLUDE(cmake/version.inc)
INCLUDE(cmake/install.inc)

ADD_SUBDIRECTORY(src)
#ADD_SUBDIRECTORY(tests)
ADD_SUBDIRECTORY(examples)

INCLUDE(CPack)