#include "tilemap.h"
#include "pti.h"

using namespace TL;

Tilemap::Tilemap() {}

Tilemap::Tilemap(int tile_width, int tile_height, int columns, int rows) {
	m_tile_width = tile_width;
	m_tile_height = tile_height;
	m_columns = columns;
	m_rows = rows;

	const size_t size = columns * rows;
	m_grid.reserve(size);
	for (int i = 0; i < size; i++) {
		m_grid.push_back(0);
	}
}

int Tilemap::tile_width() const { return m_tile_width; }

int Tilemap::tile_height() const { return m_tile_height; }

int Tilemap::columns() const { return m_columns; }

int Tilemap::rows() const { return m_rows; }

void Tilemap::set_cell(int x, int y, int tex) { m_grid[x + y * m_columns] = tex; }

void Tilemap::set_cells(int x, int y, int w, int h, int tex) {
	for (int tx = x; tx < x + w; tx++) {
		for (int ty = y; ty < y + h; ty++) {
			set_cell(tx, ty, tex);
		}
	}
}

void Tilemap::render() {
	// int x, y;
	// pti_get_camera(&x, &y);
	// pti_camera(0, 0);

	int celx = 0;
	int cely = 0;
	int sx = 0;
	int sy = 0;
	int celw = 30;
	int celh = 17;
	pti_map(celx, cely, sx, sy, celw, celh);
}
