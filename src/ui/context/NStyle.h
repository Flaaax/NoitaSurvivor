#pragma once
#include "src/utils/Integers.h"

namespace sf {
	class Font;
}

struct NStyle {
	const sf::Font& font{};
	u32 defaultFontSize = 25;
};