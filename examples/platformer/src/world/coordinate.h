#pragma once

#include <array>
#include <memory>

template<typename T>
struct CoordXY {
	static CoordXY<T> Up;
	static CoordXY<T> Right;
	static CoordXY<T> Down;
	static CoordXY<T> Left;

	T x, y;
	CoordXY() : x(0), y(0) {}
	CoordXY(T x_, T y_) : x(x_), y(y_) {}
	CoordXY(const CoordXY &) = default;
	CoordXY(CoordXY &&) = default;
	CoordXY<T> operator*(const T rhs) const {
		return {x * rhs, y * rhs};
	}
	CoordXY<T> operator+(const CoordXY<T> &rhs) const {
		return {x + rhs.x, y + rhs.y};
	}
	CoordXY<T> operator-(const CoordXY<T> &rhs) const {
		return {x - rhs.x, y - rhs.y};
	}
	CoordXY<T> &operator=(const CoordXY<T> &rhs) {
		x = rhs.x;
		y = rhs.y;
		return (*this);
	};
	bool operator<(const CoordXY &other) const {
		return x < other.x || (x == other.x && y < other.y);
	}
	bool operator==(const CoordXY &other) const {
		return (x == other.x && y == other.y);
	}
};

template<typename T>
CoordXY<T> CoordXY<T>::Up{+0, +1};
template<typename T>
CoordXY<T> CoordXY<T>::Right{+1, +0};
template<typename T>
CoordXY<T> CoordXY<T>::Down{+0, -1};
template<typename T>
CoordXY<T> CoordXY<T>::Left{-1, +0};

template class CoordXY<int>;
template class CoordXY<float>;

template<>
struct std::hash<CoordXY<int>> {
	std::size_t operator()(const CoordXY<int> &location) const noexcept {
		return std::hash<int>()(location.x ^ (location.y << 16));
	}
};