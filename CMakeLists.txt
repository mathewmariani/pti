cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(pti)

set(THIRDPARTY_DIR libs)
set(ASSIGNMENTS_DIR assignments)

# suppress warnings
add_compile_options(-Wno-c99-designator)
add_compile_options(-Wno-reorder-init-list)

# macros
include("extra/cmake/macros.cmake")

# thirdparty
# include("extra/cmake/dbgui.cmake")
include("extra/cmake/imgui.cmake")
include("extra/cmake/sokol.cmake")
include("extra/cmake/tracy.cmake")

include_directories(${THIRDPARTY_DIR})

add_subdirectory(engine)
add_subdirectory(tests)
add_subdirectory(samples)
# add_subdirectory(examples)