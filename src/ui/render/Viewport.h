#pragma once
#include "src/utils/Integers.h"
#include "src/utils/Vec2/Vec2.h"

namespace flx::ui {
	// Defines the transform from canvas to screen
	struct Viewport {
		vec2 offset;
		vec2 scale = {1.f, 1.f};

		// canvas -> screen
		sf::Transform getTransformToScreen() const {
			return sf::Transform()
				.translate(offset)
				.scale(scale);
		}

		u32 getPx(u32 designedPx) const {
			return static_cast<u32>(std::roundf(static_cast<float>(designedPx) * scale.y));
		}

		vec2 posToScreen(vec2 pos) const {
			return offset + pos * scale;
		}

		vec2 posFromScreen(vec2 screenPos) const {
			return (screenPos - offset) / scale;
		}

		rect rectToScreen(rect rect) const {
			return {posToScreen(rect.position), rect.size * scale};
		}

		rect rectFromScreen(rect rect) const {
			return {posFromScreen(rect.position), rect.size / scale};
		}
	};

	inline vec2 operator>>(vec2 pos, const Viewport& viewport) {
		return viewport.posToScreen(pos);
	}

	inline vec2 operator<<(vec2 pos, const Viewport& viewport) {
		return viewport.posFromScreen(pos);
	}

	inline rect operator>>(rect rect, const Viewport& viewport) {
		return viewport.rectToScreen(rect);
	}

	inline rect operator<<(rect rect, const Viewport& viewport) {
		return viewport.rectFromScreen(rect);
	}
}
