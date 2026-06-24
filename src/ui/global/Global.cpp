#include "Global.h"

#include "../../utils/Logging/Logger.h"

namespace flx::ui {
	const sf::Font& Global::getDefaultFont() {
		if (!defaultFont) {
			logger.error_and_throw("No default font avaliable");
		}
		return *defaultFont;
	}

	void Global::setDefaultFont(const sf::Font& font) {
		if (defaultFont) {
			logger.error_and_throw("Default font should be set once");
		}
		defaultFont = &font;
	}

	flx::Logger& Global::getLogger() {
		return logger_;
	}
}