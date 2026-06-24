#pragma once
#include "../../utils/Logging/Logger.h"

namespace sf {
	class Font;
}

namespace flx::ui {
	class Global {
	private:
		inline static const sf::Font* defaultFont{};
		inline static Logger logger_ = Logger::makeAsync("FLX_UI");

	public:
		static const sf::Font& getDefaultFont();
		static void setDefaultFont(const sf::Font& font);
		static Logger& getLogger();
	};
} // namespace flx::ui