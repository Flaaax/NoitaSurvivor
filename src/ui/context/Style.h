#pragma once
#include "src/utils/Integers.h"

namespace sf {
	class Font;
}

namespace flx::ui {

	struct Style {
		const sf::Font& font{};
		u32 defaultFontSize = 25;
	};
} // namespace flx::ui
