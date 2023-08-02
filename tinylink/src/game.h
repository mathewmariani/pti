#pragma once
#include "blah/include/blah.h"
#include "world.h"

#include <vector>

using namespace Blah;

namespace TL {
class Game {
public:
  static constexpr int width = 240;
  static constexpr int height = 135;
  static constexpr int tile_width = 8;
  static constexpr int tile_height = 8;
  static constexpr int columns = width / tile_width;
  static constexpr int rows = height / tile_height + 1;

  static inline const char *title = "SWORD II: ADVENTURE OF FROG";
  static inline const char *controls = "arrow keys + X / C\nstick + A / X";
  static inline const char *ending = "YOU SAVED POND\nAND YOU ARE\nA REAL HERO";

  World world;
  Point room;
  Vec2f camera;
  bool fullscreen = false;

  void load_room(Point cell, bool is_reload = false);
  void startup();
  void shutdown();
  void update();
  void render();
  void shake(float time);

  static int rand_int(int min, int max);

private:
  bool m_draw_colliders;
  bool m_transition = false;
  float m_next_ease;
  Point m_next_room;
  Point m_last_room;
  std::vector<Entity *> m_last_entities;
  Point m_shake;
  float m_shake_timer = 0;
};
} // namespace TL