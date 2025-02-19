project(platformer_cpp)

#=== EXECUTABLE: platformer_cpp
set(PLATFORMER_HEADERS
    src/bank.h

    src/entity/base.h
    src/entity/coin.h
    src/entity/goomba.h
    src/entity/player.h
    src/entity/registry.h

    src/lib/assets.h
    src/lib/cute_aseprite.h)

set(PLATFORMER_SRC
    src/bank.cpp

    src/entity/base.cpp
    src/entity/coin.cpp
    src/entity/goomba.cpp
    src/entity/player.cpp
    src/entity/registry.cpp
    
    src/lib/assets.cpp)

if(CMAKE_SYSTEM_NAME STREQUAL Windows)
    add_executable(platformer_cpp WIN32 ${PLATFORMER_SRC} ${PLATFORMER_HEADERS})
else()
    add_executable(platformer_cpp src/main.cpp ${PLATFORMER_SRC} ${PLATFORMER_HEADERS})
endif()

target_link_libraries(platformer_cpp engine)

# assets
add_custom_command(
  TARGET platformer_cpp
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  "${CMAKE_CURRENT_SOURCE_DIR}/assets"
  "$<TARGET_FILE_DIR:platformer_cpp>/assets")