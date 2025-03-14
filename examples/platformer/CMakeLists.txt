project(platformer)

#=== EXECUTABLE: platformer_cpp
set(PLATFORMER_HEADERS
    src/bank.h
    src/gamestate.h

    src/entity/base.h
    src/entity/coin.h
    src/entity/effect.h
    src/entity/goomba.h
    src/entity/player.h
    src/entity/projectile.h
    src/entity/registry.h
    src/entity/shooter.h

    src/lib/assets.h
    src/lib/cute_aseprite.h)

set(PLATFORMER_SRC
    src/bank.cpp
    src/gamestate.cpp

    src/entity/base.cpp
    src/entity/coin.cpp
    src/entity/effect.cpp
    src/entity/goomba.cpp
    src/entity/player.cpp
    src/entity/projectile.cpp
    src/entity/registry.cpp
    src/entity/shooter.cpp
    
    src/lib/assets.cpp)

if(CMAKE_SYSTEM_NAME STREQUAL Windows)
    add_executable(platformer WIN32 ${PLATFORMER_SRC} ${PLATFORMER_HEADERS})
else()
    add_executable(platformer src/main.cpp ${PLATFORMER_SRC} ${PLATFORMER_HEADERS})
endif()

target_link_libraries(platformer PRIVATE engine)
target_link_libraries(platformer PRIVATE imgui)
# target_link_libraries(platformer PRIVATE dbgui)
target_link_libraries(platformer PRIVATE tracy)

# assets
add_custom_command(TARGET platformer POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E create_symlink
  "${CMAKE_CURRENT_SOURCE_DIR}/assets"
  "$<TARGET_FILE_DIR:platformer>/assets")