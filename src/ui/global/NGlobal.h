#pragma once
#include "../../utils/Logging/Logger.h"

namespace sf {
	class Font;
}

namespace flx::ui {
	class NGlobal {
	private:
		inline static const sf::Font* defaultFont{};
		inline static flx::Logger logger_ = flx::Logger::makeAsync("NUI");

	public:
		static const sf::Font& getDefaultFont();
		static void setDefaultFont(const sf::Font& font);
		static flx::Logger& getLogger();
	};
} // namespace flx::ui