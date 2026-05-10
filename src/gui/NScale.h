#pragma once
#include"src/utils/Vec2.h"
#include"src/utils/Macro.h"


class NScale {
private:
	friend class NWindow;
	void updateWindowSize(const nvec2& _windowSize) {
		currentWindowSize = _windowSize;
		currentWindowRect = { 0,0,currentWindowSize.x,currentWindowSize.y };

		float windowRatio = currentWindowSize.ratio();
		constexpr float default_ratio = defaultWindowSize.ratio();

		nvec2 maxRenderSize;
		float scale;

		if (windowRatio > default_ratio) {
			scale = currentWindowSize.y / defaultWindowSize.y;
			float xOffset = (currentWindowSize.x - currentWindowSize.y * default_ratio) / 2.f;
			realRenderOffset = { xOffset,0.f };
		}
		else {
			scale = currentWindowSize.x / defaultWindowSize.x;
			float yOffset = (currentWindowSize.y - currentWindowSize.x / default_ratio) / 2.f;
			realRenderOffset = { 0.f, yOffset };
		}

		currentRenderScale = { scale,scale };
	}
public:
	N_CONSTEXPR_VAR nvec2 gameRenderScale = { 50.f,50.f };
	//N_CONSTEXPR_VAR nvec2 gameRenderScale = { 1.f,1.f };
	N_CONSTEXPR_VAR unsigned int defaultWindowSizeX = 1920;
	N_CONSTEXPR_VAR unsigned int defaultWindowSizeY = 1080;
	N_CONSTEXPR_VAR nvec2 defaultWindowSize = { defaultWindowSizeX, defaultWindowSizeY };
	N_CONSTEXPR_VAR nvec2 gameRenderOffset = defaultWindowSize / 2.f;		//to keep (0,0) to the center
	N_CONSTEXPR_VAR nrect windowSizeRect = { 0,0,defaultWindowSizeX ,defaultWindowSizeY };
	N_CONSTEXPR_VAR float player_size = 1.f;

	N_CONSTEXPR_VAR float gfx_wand_scale = 0.1f;

	nvec2 currentRenderScale = { 1,1 };
	nvec2 currentWindowSize;
	nrect currentWindowRect;
	nvec2 realRenderOffset;

	nvec2 realPosToRenderPos(const nvec2& pos) const {
		return (pos - realRenderOffset) / currentRenderScale.x;
	}
};