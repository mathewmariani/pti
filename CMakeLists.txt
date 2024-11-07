cmake_minimum_required(VERSION 3.5)

project(pti)

set(THIRDPARTY_DIR libs)
set(ASSIGNMENTS_DIR assignments)

# suppress warnings
add_compile_options(-Wno-c99-designator)
add_compile_options(-Wno-reorder-init-list)

# macros
include("cmake/macros.cmake")

# thirdparty
include("cmake/cute.cmake")
# include("cmake/imgui.cmake")
include("cmake/sokol.cmake")
include("cmake/stb.cmake")

include_directories(${THIRDPARTY_DIR})

# include engine
add_subdirectory(engine)
add_subdirectory(examples)