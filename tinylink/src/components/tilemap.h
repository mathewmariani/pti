#pragma once
#include "../world.h"
#include <vector>

namespace TL {
class Tilemap : public Component {
public:
  Tilemap();
  Tilemap(int tile_width, int tile_height, int columns, int rows);

  int tile_width() const;
  int tile_height() const;
  int columns() const;
  int rows() const;

  void set_cell(int x, int y, int tex);
  void set_cells(int x, int y, int w, int h, int tex);
  void render() override;

private:
  std::vector<int> m_grid;
  int m_tile_width = 0;
  int m_tile_height = 0;
  int m_columns = 0;
  int m_rows = 0;
};
} // namespace TL