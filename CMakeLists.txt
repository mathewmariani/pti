cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(pti)

set(PTI_DIR src)
set(THIRDPARTY_DIR libs)

# suppress warnings
add_compile_options(-Wno-c99-designator)
add_compile_options(-Wno-reorder-init-list)

# macros
include("extra/cmake/macros.cmake")

# thirdparty
include("extra/cmake/imgui.cmake")
include("extra/cmake/dbgui.cmake")
include("extra/cmake/sokol.cmake")

include_directories(${PTI_DIR})
include_directories(${THIRDPARTY_DIR})

add_subdirectory(tests)