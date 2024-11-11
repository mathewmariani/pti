set(BUILD_WITH_CUTE_DEFAULT TRUE)

option(BUILD_WITH_CUTE "cute Enabled" ${BUILD_WITH_CUTE_DEFAULT})
message("BUILD_WITH_CUTE: ${BUILD_WITH_CUTE}")

if(BUILD_WITH_CUTE)
  set(CUTE_DIR ${THIRDPARTY_DIR}/cute)
  set(CUTE_FILES
    ${CUTE_DIR}/cute.c
    ${CUTE_DIR}/cute_aseprite.h)

  add_library(cute STATIC ${CUTE_FILES})
endif()