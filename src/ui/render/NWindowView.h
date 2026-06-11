#pragma once
#include "../../utils/Vec2/Vec2.h"
#include "NViewport.h"

class NWindowView {
public:
	// Don't change the default* vals
	nvec2u defaultWindowSize = {1920, 1080};
	nvec2 canvasSize{};
	nrect canvasRect{};

	nvec2u windowSize;
	nvec2 windowSizeF;
	nrect windowRect;

	NViewport viewport;

	explicit NWindowView(nvec2u defaultWindowSize = {1920, 1080}) {
		this->defaultWindowSize = defaultWindowSize;
		canvasSize = defaultWindowSize.to<nvec2>();
		canvasRect = {0, 0, canvasSize.x, canvasSize.y};

		resize(defaultWindowSize);
	}

	nvec2 toCanvasPos(nvec2 screenPos) const {
		return (screenPos - viewport.offset) / viewport.scale.x;
	}

	void resize(nvec2u newWindowSize) {
		windowSize = newWindowSize;
		windowSizeF = newWindowSize.to<nvec2>();
		windowRect = {0, 0, windowSizeF.x, windowSizeF.y};

		const float sizeRatio = windowSizeF.ratio();
		const float defaultRatio = defaultWindowSize.to<nvec2>().ratio();

		float scale;

		if (sizeRatio > defaultRatio) {
			scale = windowSizeF.y / defaultWindowSize.y;
			float xOffset = (windowSizeF.x - windowSizeF.y * defaultRatio) / 2.f;
			viewport.offset = {xOffset, 0.f};
		} else {
			scale = windowSizeF.x / defaultWindowSize.x;
			float yOffset = (windowSizeF.y - windowSizeF.x / defaultRatio) / 2.f;
			viewport.offset = {0.f, yOffset};
		}

		viewport.scale = {scale, scale};
	}
};