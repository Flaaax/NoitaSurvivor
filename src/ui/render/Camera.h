#pragma once
#include "src/utils/Math.h"

namespace flx::ui {
	class Camera {
	public:
		vec2 renderPos = {0, 0};

		vec2 offeset(vec2 v) const {
			return v - renderPos;
		}
	};
} // namespace flx::ui