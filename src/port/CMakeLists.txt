CMAKE_MINIMUM_REQUIRED(VERSION 3.0...3.20)
PROJECT(ModbusThings)

INCLUDE_DIRECTORIES(inc)

INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR}/build/include)

INCLUDE_DIRECTORIES(${TD_COMMUNITY_DIR}/src/deps/mqttclient/mqttclient)
INCLUDE_DIRECTORIES(${TD_COMMUNITY_DIR}/src/deps/mqttclient/mqtt)
INCLUDE_DIRECTORIES(${TD_COMMUNITY_DIR}/src/deps/mqttclient/common)
INCLUDE_DIRECTORIES(${TD_COMMUNITY_DIR}/src/deps/mqttclient/common/log)
INCLUDE_DIRECTORIES(${TD_COMMUNITY_DIR}/src/deps/mqttclient/config)
INCLUDE_DIRECTORIES(${TD_COMMUNITY_DIR}/src/deps/mqttclient/network)
INCLUDE_DIRECTORIES(${TD_COMMUNITY_DIR}/src/deps/mqttclient/platform/linux)



MESSAGE(STATUS "link modbus: " ${LINK_MODBUS} " link mqtt: " ${LINK_MQTT})

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
  message(STATUS "dir='${dir}'")
endforeach()


ADD_LIBRARY(port 
    src/libcurl/portHttpClient.c
    src/libmodbus/portMaster.c
    src/mqttclient/portMqtt.c
    src/simpledb/portDB.c
    src/ThingsBoard/portServer.c
)


TARGET_LINK_LIBRARIES(port cJson libcurl mutils ${LINK_MODBUS} ${LINK_MQTT})


get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
  message(STATUS "dir='${dir}'")
endforeach()






