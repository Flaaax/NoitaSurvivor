#pragma once
#include "../../utils/Vec2/Vec2.h"
#include "src/utils/Macro.h"

class NWindowViewport {
public:
	explicit NWindowViewport(nvec2u defaultWindowSize = {1920, 1080}) {
		this->defaultWindowSize = defaultWindowSize;
		defaultWindowSizeF = defaultWindowSize.to<nvec2>();
		defaultWindowSizeRect = {0, 0, defaultWindowSizeF.x, defaultWindowSizeF.y};

		update(defaultWindowSizeF);
	}

	// Don't change the default* vals
	nvec2u defaultWindowSize = {1920, 1080};
	nvec2 defaultWindowSizeF{};
	nrect defaultWindowSizeRect{};

	nvec2 scale = {1, 1};
	nvec2 windowSize;
	nrect windowRect;
	nvec2 offset;

	nvec2 toRenderPos(nvec2 screenPos) const {
		return (screenPos - offset) / scale.x;
	}

	void update(nvec2 newWindowSize) {
		windowSize = newWindowSize;
		windowRect = {0, 0, windowSize.x, windowSize.y};

		const float sizeRatio = windowSize.ratio();
		constexpr float defaultRatio = defaultWindowSize.to<nvec2>().ratio();

		float scale;

		if (sizeRatio > defaultRatio) {
			scale = windowSize.y / defaultWindowSize.y;
			float xOffset = (windowSize.x - windowSize.y * defaultRatio) / 2.f;
			offset = {xOffset, 0.f};
		} else {
			scale = windowSize.x / defaultWindowSize.x;
			float yOffset = (windowSize.y - windowSize.x / defaultRatio) / 2.f;
			offset = {0.f, yOffset};
		}

		this->scale = {scale, scale};
	}
};