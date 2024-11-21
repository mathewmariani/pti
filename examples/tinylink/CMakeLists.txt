cmake_minimum_required(VERSION 3.0)
project(tinylink)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#=== LIBRARY: sokol
# add headers to the the file list because they are useful to have in IDEs
set(BLAH_SRC
src/blah/src/blah_filesystem.cpp
src/blah/src/blah_time.cpp
src/blah/src/blah_stream.cpp
# src/blah/src/blah_string.cpp
src/blah/src/blah_subtexture.cpp
src/blah/src/blah_aseprite.cpp
src/blah/src/blah_image.cpp
src/blah/src/blah_packer.cpp
src/blah/src/internal/blah_platform.cpp
)


add_library(blah STATIC ${BLAH_SRC})
target_include_directories(blah
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/blah/include>
	PRIVATE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/blah/src>
)

#=== EXECUTABLE: tinylink
set(TINYLINK_HEADERS
    src/bank.h
    src/factory.h
    src/game.h
    src/world.h

    src/cute_aseprite.h

    src/assets/sprite.h

    src/components/animator.h
    src/components/collider.h
    src/components/enemy.h
    src/components/ghost_frog.h
    src/components/hurtable.h
    src/components/mover.h
    src/components/orb.h
    src/components/player.h
    src/components/tilemap.h
    src/components/timer.h)

set(TINYLINK_SRC
    src/bank.cpp
    src/factory.cpp
    src/game.cpp
    src/world.cpp

    src/assets/sprite.cpp

    src/components/animator.cpp
    src/components/collider.cpp
    src/components/ghost_frog.cpp
    src/components/hurtable.cpp
    src/components/mover.cpp
    src/components/orb.cpp
    src/components/player.cpp
    src/components/tilemap.cpp
    src/components/timer.cpp)

if(CMAKE_SYSTEM_NAME STREQUAL Windows)
    add_executable(tinylink WIN32 ${TINYLINK_SRC} ${TINYLINK_HEADERS})
else()
    add_executable(tinylink src/main.cpp ${TINYLINK_SRC} ${TINYLINK_HEADERS})
endif()

target_link_libraries(tinylink engine)
target_link_libraries(tinylink blah)

# assets
add_custom_command(
  TARGET tinylink
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  "${CMAKE_CURRENT_SOURCE_DIR}/assets"
  "$<TARGET_FILE_DIR:tinylink>/assets")