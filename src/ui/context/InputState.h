#pragma once
#include "../../utils/Vec2/Vec2.h"
#include "src/utils/Container/Vector.h"

namespace flx::ui {
	struct InputState {
		vec2 mouseScreen;
		vec2 mouseRender;

		Array<bool, 3> mouseButtons;

		// TODO add more state...
	};
}

