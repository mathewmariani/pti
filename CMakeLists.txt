cmake_minimum_required(VERSION 3.5)

set(THIRDPARTY_DIR libs)

# macros
include("cmake/macros.cmake")
include("cmake/sokol.cmake")

include_directories(${THIRDPARTY_DIR})

# include engine
add_subdirectory(src)
add_subdirectory(examples)