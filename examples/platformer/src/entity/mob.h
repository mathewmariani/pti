#pragma once

#include "base.h"

enum class MobState : uint8_t {
	Normal = 0,
};

enum MobFlags : uint8_t {
	MOB_FLAGS_NORMAL = (1 << 0),
	MOB_FLAGS_DESTROYED = (1 << 1),
};

struct Mob : EntityBase {
	MobState state;
	uint8_t flags;
};