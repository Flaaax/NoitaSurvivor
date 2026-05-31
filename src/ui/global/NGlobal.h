#pragma once

namespace sf {
	class Font;
}

class NGlobal {
private:
	inline static const sf::Font* defaultFont{};

public:
	static const sf::Font& getDefaultFont();
	static void setDefaultFont(const sf::Font& font);
};