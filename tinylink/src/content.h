#pragma once

#include <string>

namespace TL {

struct Sprite;
struct Tileset;

class Content {
public:
  static const Sprite *find_sprite(const std::string &name);
};
} // namespace TL