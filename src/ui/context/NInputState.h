#pragma once
#include "../../utils/Vec2/Vec2.h"
#include "src/utils/Container/Vector.h"
#include <SFML/Window/Mouse.hpp>

struct NInputState {
	nvec2 mouseScreen;
	nvec2 mouseRender;

	Util::Array<bool, 3> mouseButtons;

	// TODO add more state...
};