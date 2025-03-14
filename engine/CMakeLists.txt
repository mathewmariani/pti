add_library(engine STATIC platform.c)
target_include_directories(engine INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(engine PUBLIC ${THIRDPARTY_DIR})
target_link_libraries(engine PRIVATE sokol)