#define SOKOL_IMPL
#define SOKOL_NO_ENTRY

#if defined(_MSC_VER)
#define SOKOL_GLCORE33
#elif defined(__EMSCRIPTEN__)
#define SOKOL_GLES2
#elif defined(__APPLE__)
#define SOKOL_GLCORE33
#else
#define SOKOL_GLCORE33
#endif

#include "sokol_app.h"
#include "sokol_audio.h"
#include "sokol_gfx.h"
#include "sokol_glue.h"
#include "sokol_log.h"
