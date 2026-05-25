#pragma once
#include "src/utils/Integers.h"

namespace sf {
	class Font;
}

struct NTooltipStyle {
	const sf::Font* font{};
	u32 titleSize = 30;
	u32 contentSize = 25;
	u32 flavorSize = 20;
};

struct NStyle {
	NTooltipStyle tooltipStyle;
	const sf::Font* font{};
};