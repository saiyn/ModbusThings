PROJECT(ModbusThings)

IF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  CMAKE_MINIMUM_REQUIRED(VERSION 3.0...3.20)
ELSE ()
  CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
ENDIF ()



ADD_SUBDIRECTORY(cJson)



IF (TD_LINUX_64 AND JEMALLOC_ENABLED)
  MESSAGE("")
  MESSAGE("${Green} ENABLE jemalloc ${ColourReset}")
  MESSAGE("")
  MESSAGE("setup deps/jemalloc, current source dir:" ${CMAKE_CURRENT_SOURCE_DIR})
  MESSAGE("binary dir:" ${CMAKE_BINARY_DIR})
  include(ExternalProject)
  ExternalProject_Add(jemalloc
    PREFIX        "jemalloc"
    SOURCE_DIR    ${CMAKE_CURRENT_SOURCE_DIR}/jemalloc
    BUILD_IN_SOURCE     1
    CONFIGURE_COMMAND   ./autogen.sh COMMAND ./configure --prefix=${CMAKE_BINARY_DIR}/build/
    BUILD_COMMAND       ${MAKE}
  )
  INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/build/include)
ENDIF ()
