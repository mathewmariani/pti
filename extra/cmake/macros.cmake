macro(emscripten target)
  if (CMAKE_SYSTEM_NAME STREQUAL Emscripten)
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
      set(CMAKE_EXECUTABLE_SUFFIX ".js")
    else()
      set(CMAKE_EXECUTABLE_SUFFIX ".html")
    endif()
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