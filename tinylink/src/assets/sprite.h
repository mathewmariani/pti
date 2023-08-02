#pragma once
#include "../blah/include/blah.h"
#include <string>
#include <vector>

using namespace Blah;

namespace TL {
struct Sprite {
  struct iFrame {
    int n, w, h;
  };
  struct Frame {
    iFrame image;
    float duration;
  };

  struct Animation {
    std::string name;
    std::vector<Frame> frames;

    float duration() const {
      float d = 0;
      for (auto &it : frames)
        d += it.duration;
      return d;
    }
  };

  std::string name;
  Vec2f origin;
  std::vector<Animation> animations;

  const Animation *get_animation(const std::string &name) const;
};
} // namespace TL