cmake_minimum_required(VERSION 3.5)
project(platformer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#=== EXECUTABLE: platformer
set(PLATFORMER_HEADERS
    src/game.h
    
    src/entities/entities.h

    src/lib/assets.h
    src/lib/collisions.h
    src/lib/cute_aseprite.h
    src/lib/entity.h)

set(PLATFORMER_SRC
    src/game.cpp

    src/entities/bullet.cpp
    src/entities/coin.cpp
    src/entities/goomba.cpp
    src/entities/player.cpp
    src/entities/pop.cpp
    src/entities/shooter.cpp
    
    src/lib/assets.cpp
    src/lib/collisions.cpp
    src/lib/entity.cpp)

if(CMAKE_SYSTEM_NAME STREQUAL Windows)
    add_executable(platformer WIN32 ${PLATFORMER_SRC} ${PLATFORMER_HEADERS})
else()
    add_executable(platformer src/main.cpp ${PLATFORMER_SRC} ${PLATFORMER_HEADERS})
endif()

target_include_directories(engine PUBLIC include)
target_link_libraries(platformer engine)

if (CMAKE_SYSTEM_NAME STREQUAL Emscripten)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    target_link_options(platformer PUBLIC --shell-file ../../sokol/shell.html)
    target_link_options(platformer PUBLIC -sNO_FILESYSTEM=1 -sASSERTIONS=0 -sMALLOC=emmalloc --closure=1)
endif()

# explicitly strip dead code
if (CMAKE_C_COMPILER_ID MATCHES "Clang" AND NOT CMAKE_SYSTEM_NAME STREQUAL Emscripten)
    target_link_options(platformer PRIVATE LINKER:-dead_strip)
endif()