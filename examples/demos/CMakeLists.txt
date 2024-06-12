macro(pti_demo target file)
  add_executable(${target} ${file})
  target_link_libraries(${target} PRIVATE engine)
endmacro()

pti_demo(rosecurve rosecurve.c)
pti_demo(moire moire.c)
pti_demo(mosaic mosaic.c)