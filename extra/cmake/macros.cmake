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

macro(copy_assets target)
  # assets
  add_custom_command(
    TARGET ${target}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:${target}>/assets)
endmacro()

macro(pti_executable target files)
  add_executable(${target} ${files})
  target_link_libraries(${target} PRIVATE engine)
  emscripten(${target})
endmacro()