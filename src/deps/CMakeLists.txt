PROJECT(ModbusThings)

IF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  CMAKE_MINIMUM_REQUIRED(VERSION 3.0...3.20)
ELSE ()
  CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
ENDIF ()



ADD_SUBDIRECTORY(cJson)
ADD_SUBDIRECTORY(curl)


IF (TD_LINUX_64)
  MESSAGE("")
  MESSAGE("setup deps/libmodbus, current source dir:" ${CMAKE_CURRENT_SOURCE_DIR})
  MESSAGE("binary dir:" ${CMAKE_BINARY_DIR})
  include(ExternalProject)
  ExternalProject_Add(libmodbus
    PREFIX        "libmodbus"
    SOURCE_DIR    ${CMAKE_CURRENT_SOURCE_DIR}/libmodbus
    BUILD_IN_SOURCE     1
    CONFIGURE_COMMAND   ./configure --prefix=${CMAKE_BINARY_DIR}/build/
    BUILD_COMMAND       ${MAKE}
  )
  #TARGET_INCLUDE_DIRECTORIES(modbus PUBLIC ${CMAKE_BINARY_DIR}/build/include)
ENDIF ()



ADD_SUBDIRECTORY(mqttclient)


