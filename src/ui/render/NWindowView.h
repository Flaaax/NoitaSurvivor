#pragma once
#include "../../utils/Vec2/Vec2.h"
#include "NViewport.h"

namespace flx::ui {
	struct NWindowView {
	public:
		// Don't change the default* vals
		vec2u defaultWindowSize = {1920, 1080};
		vec2 canvasSize{};
		rect canvasRect{};

		vec2u windowSize;
		vec2 windowSizeF;
		rect windowRect;

		NViewport viewport;

		explicit NWindowView(vec2u defaultWindowSize = {1920, 1080}) {
			this->defaultWindowSize = defaultWindowSize;
			canvasSize = defaultWindowSize.to<vec2>();
			canvasRect = {0, 0, canvasSize.x, canvasSize.y};

			resize(defaultWindowSize);
		}

		vec2 toCanvasPos(vec2 screenPos) const {
			return (screenPos - viewport.offset) / viewport.scale.x;
		}

		void resize(vec2u newWindowSize) {
			windowSize = newWindowSize;
			windowSizeF = newWindowSize.to<vec2>();
			windowRect = {0, 0, windowSizeF.x, windowSizeF.y};

			const float sizeRatio = windowSizeF.ratio();
			const float defaultRatio = defaultWindowSize.to<vec2>().ratio();

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
} // namespace flx::ui