CMAKE_MINIMUM_REQUIRED(VERSION 3.0...3.20)
PROJECT(ModbusThings)


include_directories(${TD_COMMUNITY_DIR}/include)

add_executable(ThingsBoardAgent thingsboard_agent.c)

TARGET_LINK_LIBRARIES(ThingsBoardAgent core)