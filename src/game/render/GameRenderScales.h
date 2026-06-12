#pragma once
#include "src/utils/Vec2/Vec2.h"

struct GameRenderScales {
	nvec2 scale = {50.f, 50.f};
	nvec2 offset{}; // = defaultWindowSizeF / 2.f; // to keep (0,0) to the center
	float player_size = 1.f;
	float gfx_wand_scale = 0.1f;
};