#pragma once
#include "../blah/include/blah.h"
#include "../world.h"

using namespace Blah;

namespace TL {
class Orb : public Component {
public:
  float speed = 40;
  bool towards_player = true;

  Point target() const;
  void update() override;
  void destroyed() override;
  void on_hit();
};
} // namespace TL