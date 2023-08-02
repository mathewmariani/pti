cmake_minimum_required(VERSION 3.0)
project(pti)

# include engine
add_subdirectory(engine)

# add_subdirectory(tinylink)
add_subdirectory(platformer)

# add_executable(00_setup "tests/00_setup.c")
# add_executable(01_pset "tests/01_pset.c")
# add_executable(02_line "tests/02_line.c")
# add_executable(03_rectf "tests/03_rectf.c")
# add_executable(05_spr "tests/05_spr.c")
# add_executable(06_print "tests/06_print.c")
# add_executable(07_sprites "tests/07_sprites.c")
# add_executable(08_tilemap "tests/08_tilemap.c")
# add_executable(10_rosecurve "tests/10_rosecurve.c")
# add_executable(11_moire "tests/11_moire.c")
# add_executable(12_mosaic "tests/12_mosaic.c")
# add_executable(20_topdown "tests/20_topdown.c")
# add_executable(21_tetris "tests/21_tetris.c")
# add_executable(22_snake "tests/22_snake.c")
# add_executable(23_platformer "tests/23_platformer.c")

get_property(targets DIRECTORY "${_dir}" PROPERTY BUILDSYSTEM_TARGETS)
foreach(target IN LISTS targets)
    target_link_libraries(${target} LINK_PUBLIC engine)

    if (CMAKE_SYSTEM_NAME STREQUAL Emscripten)
        set(CMAKE_EXECUTABLE_SUFFIX ".html")
        target_link_options(${target} PUBLIC --shell-file ../../sokol/shell.html)
        target_link_options(${target} PUBLIC -sNO_FILESYSTEM=1 -sASSERTIONS=0 -sMALLOC=emmalloc --closure=1)
    endif()

    # explicitly strip dead code
    if (CMAKE_C_COMPILER_ID MATCHES "Clang" AND NOT CMAKE_SYSTEM_NAME STREQUAL Emscripten)
        target_link_options(${target} PRIVATE LINKER:-dead_strip)
    endif()
endforeach()