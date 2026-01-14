#pragma once

#include <stdint.h>

// sweetie16 palette
static uint32_t __sweetie16[] = {
		0x1a1c2cff,
		0x5d275dff,
		0xb13e53ff,
		0xef7d57ff,
		0xffcd75ff,
		0xa7f070ff,
		0x38b764ff,
		0x257179ff,
		0x29366fff,
		0x3b5dc9ff,
		0x41a6f6ff,
		0x73eff7ff,
		0xf4f4f4ff,
		0x94b0c2ff,
		0x566c86ff,
		0x333c57ff,
};

static pti_palette_t sweetie16 = (pti_palette_t) {
		.count = 16,
		.colors = &__sweetie16[0],
};
