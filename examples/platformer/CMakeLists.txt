project(platformer)

#=== EXECUTABLE: platformer
set(PLATFORMER_HEADERS
    src/game.h
    src/bank.h
    
    src/entities/entities.h

    src/lib/assets.h
    src/lib/collisions.h
    src/lib/cute_aseprite.h
    src/lib/entity.h)

set(PLATFORMER_SRC
    src/game.cpp
    src/bank.cpp

    src/entities/bullet.cpp
    src/entities/coin.cpp
    src/entities/goomba.cpp
    src/entities/player.cpp
    src/entities/pop.cpp
    src/entities/shooter.cpp
    
    src/lib/assets.cpp
    src/lib/collisions.cpp
    src/lib/entity.cpp)

# set(PLATFORMER_FILES
#     ${PLATFORMER_SRC}
#     ${PLATFORMER_HEADERS})
# pti_executable(platformer ${PLATFORMER_FILES})

if(CMAKE_SYSTEM_NAME STREQUAL Windows)
    add_executable(platformer WIN32 ${PLATFORMER_SRC} ${PLATFORMER_HEADERS})
else()
    add_executable(platformer src/main.cpp ${PLATFORMER_SRC} ${PLATFORMER_HEADERS})
endif()

target_link_libraries(platformer engine)

# assets
add_custom_command(
  TARGET platformer
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  "${CMAKE_CURRENT_SOURCE_DIR}/assets"
  "$<TARGET_FILE_DIR:platformer>/assets")