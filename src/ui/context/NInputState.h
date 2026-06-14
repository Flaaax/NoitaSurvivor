#pragma once
#include "../../utils/Vec2/Vec2.h"
#include "src/utils/Container/Vector.h"

namespace flx::ui {
	struct NInputState {
		vec2 mouseScreen;
		vec2 mouseRender;

		flx::Array<bool, 3> mouseButtons;

		// TODO add more state...
	};
}

