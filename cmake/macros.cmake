macro(emscripten target)
  if (CMAKE_SYSTEM_NAME STREQUAL Emscripten)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    target_link_options(${target} PRIVATE
      -sINITIAL_MEMORY=50MB
      -sMAXIMUM_MEMORY=200MB
      -sALLOW_MEMORY_GROWTH=1
      -sUSE_WEBGL2=1
      $<$<CONFIG:Debug>:-g>
      $<$<CONFIG:Debug>:--shell-file ../../extra/shell.html>)
  endif()
endmacro()

macro(copy_assets)
  execute_process(
    COMMAND ${CMAKE_COMMAND} -E create_symlink
    "${CMAKE_SOURCE_DIR}/assets"
    "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/assets"
  )
endmacro()

macro(pti_executable target files)
  add_executable(${target} ${files})
  target_link_libraries(${target} PRIVATE engine)
endmacro()