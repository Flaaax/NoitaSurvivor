#include "NGlobal.h"

#include "../../utils/Logging/Logger.h"

namespace flx::ui {
	const sf::Font& NGlobal::getDefaultFont() {
		if (!defaultFont) {
			logger.error_and_throw("No default font avaliable");
		}
		return *defaultFont;
	}

	void NGlobal::setDefaultFont(const sf::Font& font) {
		if (defaultFont) {
			logger.error_and_throw("Default font should be set once");
		}
		defaultFont = &font;
	}

	flx::Logger& NGlobal::getLogger() {
		return logger_;
	}
}