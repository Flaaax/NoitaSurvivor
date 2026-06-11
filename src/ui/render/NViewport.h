#pragma once
#include "src/utils/Integers.h"
#include "src/utils/Vec2/Vec2.h"

namespace flx::ui {}

// Defines the transform from canvas to screen
struct NViewport {
	nvec2 offset;
	nvec2 scale = {1.f, 1.f};

	// canvas -> screen
	sf::Transform getTransformToScreen() const {
		return sf::Transform()
			.translate(offset)
			.scale(scale);
	}

	u32 getPx(u32 designedPx) const {
		return static_cast<u32>(std::roundf(designedPx * scale.y));
	}

	nvec2 posToScreen(nvec2 pos) const {
		return offset + pos * scale;
	}

	nvec2 posFromScreen(nvec2 screenPos) const {
		return (screenPos - offset) / scale;
	}

	nrect rectToScreen(nrect rect) const {
		return {posToScreen(rect.position), rect.size * scale};
	}

	nrect rectFromScreen(nrect rect) const {
		return {posFromScreen(rect.position), rect.size / scale};
	}
};

inline nvec2 operator>>(nvec2 pos, const NViewport& viewport) {
	return viewport.posToScreen(pos);
}

inline nvec2 operator<<(nvec2 pos, const NViewport& viewport) {
	return viewport.posFromScreen(pos);
}

inline nrect operator>>(nrect rect, const NViewport& viewport) {
	return viewport.rectToScreen(rect);
}

inline nrect operator<<(nrect rect, const NViewport& viewport) {
	return viewport.rectFromScreen(rect);
}
