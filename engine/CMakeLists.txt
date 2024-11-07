cmake_minimum_required(VERSION 3.5)
project(engine)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

#=== LIBRARY: engine
add_library(engine STATIC platform.c pti.h)
target_include_directories(engine INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(engine PUBLIC ${THIRDPARTY_DIR})
target_link_libraries(engine PRIVATE sokol)