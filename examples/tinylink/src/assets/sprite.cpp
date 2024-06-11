#include "sprite.h"

using namespace TL;

const Sprite::Animation *
TL::Sprite::get_animation(const std::string &name) const {
  for (auto &it : animations)
    if (it.name == name)
      return &it;
  return nullptr;
}
