#include "collider.h"
#include "pti.h"

using namespace TL;

Collider::Collider() {
	visible = false;
	active = false;
}

Collider Collider::make_rect(const Recti &rect) {
	Collider collider;
	collider.m_shape = Shape::Rect;
	collider.m_rect = rect;
	return collider;
}

Collider Collider::make_grid(int tile_size, int columns, int rows) {
	Collider collider;
	collider.m_shape = Shape::Grid;
	collider.m_grid.tile_size = tile_size;
	collider.m_grid.columns = columns;
	collider.m_grid.rows = rows;

	const size_t size = columns * rows;
	collider.m_grid.cells.reserve(size);
	for (int i = 0; i < size; i++) {
		collider.m_grid.cells.push_back(false);
	}

	return collider;
}

Collider::Shape Collider::shape() const { return m_shape; }

Recti Collider::get_rect() const {
	BLAH_ASSERT(m_shape == Shape::Rect, "Collider is not a Rectangle");
	return m_rect;
}

void Collider::set_rect(const Recti &value) {
	BLAH_ASSERT(m_shape == Shape::Rect, "Collider is not a Rectangle");
	m_rect = value;
}

bool Collider::get_cell(int x, int y) const {
	BLAH_ASSERT(m_shape == Shape::Grid, "Collider is not a Grid");
	BLAH_ASSERT(x >= 0 && y >= 0 && x < m_grid.columns && y < m_grid.rows, "Cell is out of bounds");

	return m_grid.cells[x + y * m_grid.columns];
}

void Collider::set_cell(int x, int y, bool value) {
	BLAH_ASSERT(m_shape == Shape::Grid, "Collider is not a Grid");
	BLAH_ASSERT(x >= 0 && y >= 0 && x < m_grid.columns && y < m_grid.rows, "Cell is out of bounds");

	m_grid.cells[x + y * m_grid.columns] = value;
}

void Collider::set_cells(int x, int y, int w, int h, bool value) {
	for (int tx = x; tx < x + w; tx++)
		for (int ty = y; ty < y + h; ty++)
			m_grid.cells[tx + ty * m_grid.columns] = value;
}

bool Collider::check(uint32_t mask, Point offset) const {
	return first(mask, offset) != nullptr;
}

Collider *Collider::first(uint32_t mask, Point offset) {
	if (world()) {
		auto other = world()->first<Collider>();
		while (other) {
			if (other != this && (other->mask & mask) == mask && overlaps(other, offset))
				return other;

			other = (Collider *) other->next();
		}
	}

	return nullptr;
}

const Collider *Collider::first(uint32_t mask, Point offset) const {
	if (world()) {
		auto other = world()->first<Collider>();
		while (other) {
			if (other != this && (other->mask & mask) == mask && overlaps(other, offset))
				return other;

			other = (Collider *) other->next();
		}
	}

	return nullptr;
}

bool Collider::overlaps(const Collider *other, Point offset) const {
	if (m_shape == Shape::Rect) {
		if (other->m_shape == Shape::Rect) {
			return rect_to_rect(this, other, offset);
		} else if (other->m_shape == Shape::Grid) {
			return rect_to_grid(this, other, offset);
		}
	} else if (m_shape == Shape::Grid) {
		if (other->m_shape == Shape::Rect) {
			return rect_to_grid(other, this, -offset);
		} else if (other->m_shape == Shape::Grid) {
			BLAH_ASSERT(false, "Grid->Grid Overlap checks not supported!");
		}
	}

	return false;
}

void Collider::render() {
	if (m_shape == Shape::Rect) {
		pti_rect(m_rect.x + entity()->position.x, m_rect.y + entity()->position.y, 16, 16, 0xFF0000FF);
	} else if (m_shape == Shape::Grid) {
		for (int x = 0; x < m_grid.columns; x++) {
			for (int y = 0; y < m_grid.rows; y++) {
				if (!m_grid.cells[x + y * m_grid.columns])
					continue;
				pti_rect(x * m_grid.tile_size, y * m_grid.tile_size, m_grid.tile_size, m_grid.tile_size, 0xFF0000FF);
			}
		}
	}
}

bool TL::Collider::rect_to_rect(const Collider *a, const Collider *b, Point offset) {
	Recti ar = a->m_rect + a->entity()->position + offset;
	Recti br = b->m_rect + b->entity()->position;

	return ar.overlaps(br);
}

bool TL::Collider::rect_to_grid(const Collider *a, const Collider *b, Point offset) {
	// get a relative rectangle to the grid
	Recti rect = a->m_rect + a->entity()->position + offset - b->entity()->position;

	// get the cells the rectangle overlaps
	int left = Calc::clamp((int) Calc::floor(rect.x / (float) b->m_grid.tile_size), 0, b->m_grid.columns);
	int right = Calc::clamp((int) Calc::ceiling(rect.right() / (float) b->m_grid.tile_size), 0, b->m_grid.columns);
	int top = Calc::clamp((int) Calc::floor(rect.y / (float) b->m_grid.tile_size), 0, b->m_grid.rows);
	int bottom = Calc::clamp((int) Calc::ceiling(rect.bottom() / (float) b->m_grid.tile_size), 0, b->m_grid.rows);

	// check each cell
	for (int x = left; x < right; x++)
		for (int y = top; y < bottom; y++)
			if (b->m_grid.cells[x + y * b->m_grid.columns])
				return true;

	// all cells were empty
	return false;
}
