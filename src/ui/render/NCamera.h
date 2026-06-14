#pragma once
#include "src/utils/Math.h"
#include <SFML/System/Vector2.hpp>

namespace flx::ui {
	class NCamera {
	public:
		vec2 renderPos = {0, 0};

		vec2 offeset(vec2 v) const {
			return v - renderPos;
		}
	};
} // namespace flx::ui