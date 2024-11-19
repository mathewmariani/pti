#pragma once

class Entity;
class Collisions {
public:
	static bool CanWiggle(Entity &a);
	static bool IsGroudned(const Entity &a);

	static bool Overlaps(const Entity &a, const Entity &b);
	static bool PlaceMeeting(const Entity &a, int dx, int dy);
};