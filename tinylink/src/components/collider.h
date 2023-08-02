#pragma once
#include "../blah/include/blah.h"
#include "../world.h"
#include <vector>

using namespace Blah;

namespace TL {
class Collider : public Component {
public:
  enum class Shape { None, Rect, Grid };

  uint32_t mask = 0;

  Collider();

  static Collider make_rect(const Recti &rect);
  static Collider make_grid(int tile_size, int columns, int rows);

  Shape shape() const;
  Recti get_rect() const;
  void set_rect(const Recti &value);
  bool get_cell(int x, int y) const;
  void set_cell(int x, int y, bool value);
  void set_cells(int x, int y, int w, int h, bool value);

  Collider *first(uint32_t mask, Point offset = Point::zero);
  const Collider *first(uint32_t mask, Point offset = Point::zero) const;
  bool check(uint32_t mask, Point offset = Point::zero) const;
  bool overlaps(const Collider *other, Point offset = Point::zero) const;

  void render() override;

private:
  struct Grid {
    int columns;
    int rows;
    int tile_size;
    std::vector<bool> cells;
  };

  Shape m_shape = Shape::None;
  Recti m_rect;
  Grid m_grid;

  static bool rect_to_rect(const Collider *a, const Collider *b, Point offset);
  static bool rect_to_grid(const Collider *a, const Collider *b, Point offset);
};
} // namespace TL