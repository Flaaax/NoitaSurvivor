#pragma once
#include "src/ui/render/Viewport.h"

namespace flx::game {
	class Camera {
	public:
		vec2 position; // world position
		vec2 scale = {1.f, 1.f};

		// void update(float dt) {
		//
		// }
	};
} // namespace flx::game