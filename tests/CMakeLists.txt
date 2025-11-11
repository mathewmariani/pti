macro(pti_executable target files)
  add_executable(${target} platform_gl.cpp assets.c ${files})

  target_include_directories(${target} PRIVATE ${PTI_DIR})
  target_include_directories(${target} PRIVATE ${THIRDPARTY_DIR})
  target_link_libraries(${target} PRIVATE sokol)

  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DPTI_TRACE_HOOKS)
    add_definitions(-DPTI_DEBUG)

    target_link_libraries(${target} PRIVATE dbgui)
    target_link_libraries(${target} PRIVATE imgui)
  endif()

  emscripten(${target})
endmacro()

pti_executable(test_setup test_setup.c)
pti_executable(test_pset test_pset.c)
pti_executable(test_line test_line.c)
pti_executable(test_rect test_rect.c)
pti_executable(test_rectf test_rectf.c)
pti_executable(test_circ test_circ.c)
pti_executable(test_circf test_circf.c)
pti_executable(test_print test_print.c)
pti_executable(test_spr test_spr.c)
pti_executable(test_map test_map.c)

# Make sure the tests build dir always has assets/
add_custom_target(tests_assets_symlink ALL
  COMMAND ${CMAKE_COMMAND} -E create_symlink
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    "${CMAKE_CURRENT_BINARY_DIR}/assets"
)
