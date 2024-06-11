#include "blah_platform.h"

// for File Reading / Writing
#include <filesystem>

// Windows requires a few extra includes
#if _WIN32
#define WIN32_LEAN_AND_MEAN
#include <SDL_syswm.h> // for SDL_SysWMinfo for D3D11
#include <shellapi.h>  // for ShellExecute for dir_explore
#include <windows.h>   // for the following includes
#endif

// Macro defined by X11 conflicts with MouseButton enum
#undef None

using namespace Blah;

bool Platform::init(const Config &config) { return true; }

void Platform::ready() {}

void Platform::shutdown() {}

u64 Platform::ticks() { return 0.0; }

void Platform::update() {}

void Platform::sleep(int milliseconds) {}

void Platform::present() {}

void Platform::set_app_flags(u32 flags) {}

const char *Platform::get_title() { return "Hello world!"; }

void Platform::set_title(const char *title) {}

void Platform::get_position(int *x, int *y) {}

void Platform::set_position(int x, int y) {}

bool Platform::get_focused() { return true; }

void Platform::get_size(int *width, int *height) {}

void Platform::set_size(int width, int height) {}

void Platform::get_draw_size(int *width, int *height) {}

const char *Platform::app_path() { return ""; }

const char *Platform::user_path() { return ""; }

FileRef Platform::file_open(const char *path, FileMode mode) {
  const char *sdl_mode = "";

  switch (mode) {
  case FileMode::OpenRead:
    sdl_mode = "rb";
    break;
  case FileMode::Open:
    sdl_mode = "r+b";
    break;
  case FileMode::CreateWrite:
    sdl_mode = "wb";
    break;
  case FileMode::Create:
    sdl_mode = "w+b";
    break;
  }
  return FileRef();
}

bool Platform::file_exists(const char *path) {
  // return std::filesystem::is_regular_file(path);
  return true;
}

bool Platform::file_delete(const char *path) {
  // return std::filesystem::remove(path);
  return true;
}

bool Platform::dir_create(const char *path) {
  // return std::filesystem::create_directories(path);
  return true;
}

bool Platform::dir_exists(const char *path) {
  // return std::filesystem::is_directory(path);
  return true;
}

bool Platform::dir_delete(const char *path) {
  // return std::filesystem::remove_all(path) > 0;
  return true;
}

void Platform::dir_enumerate(Vector<FilePath> &list, const char *path,
                             bool recursive) {
  // if (std::filesystem::is_directory(path)) {
  //   if (recursive) {
  //     for (auto &p : std::filesystem::recursive_directory_iterator(path))
  //       list.emplace_back(p.path().string().c_str());
  //   } else {
  //     for (auto &p : std::filesystem::directory_iterator(path))
  //       list.emplace_back(p.path().string().c_str());
  //   }
  // }
}

void Platform::dir_explore(const char *path) {
#if _WIN32

  ShellExecute(NULL, "open", path, NULL, NULL, SW_SHOWDEFAULT);

#elif __linux__

  system(String::fmt("xdg-open \"%s\"", path).cstr());

#else

  system(String::fmt("open \"%s\"", path).cstr());

#endif
}

void Platform::set_clipboard(const char *text) {}

const char *Platform::get_clipboard() {
  // free previous clipboard text
  return "empty";
}

void *Platform::gl_get_func(const char *name) { return nullptr; }

void *Platform::gl_context_create() { return nullptr; }

void Platform::gl_context_make_current(void *context) {}

void Platform::gl_context_destroy(void *context) {}

void *Platform::d3d11_get_hwnd() { return nullptr; }

void Platform::open_url(const char *url) {}
