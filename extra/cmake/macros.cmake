macro(emscripten target)
  if (CMAKE_SYSTEM_NAME STREQUAL Emscripten)
    set(CMAKE_EXECUTABLE_SUFFIX ".js")
    target_link_options(${target} PRIVATE
      --shell-file ../../extra/shell.html
      -sINITIAL_MEMORY=50MB
      -sMAXIMUM_MEMORY=200MB
      -sALLOW_MEMORY_GROWTH=1
      -sUSE_WEBGL2=1
      -sSINGLE_FILE=1
      $<$<CONFIG:Debug>:-g>)
  endif()
endmacro()

macro(copy_assets target)
  # assets
  add_custom_command(TARGET ${target} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:${target}>/assets")
endmacro()

macro(pti_executable target files)
  add_executable(${target} ${files})
  target_link_libraries(${target} PRIVATE engine)
  emscripten(${target})
endmacro()