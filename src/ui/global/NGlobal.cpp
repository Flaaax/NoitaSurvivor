#include "NGlobal.h"

#include "src/utils/Logger.h"

const sf::Font& NGlobal::getDefaultFont() {
	if (!defaultFont) {
		Logger::error_and_throw("No default font avaliable");
	}
	return *defaultFont;
}

void NGlobal::setDefaultFont(const sf::Font& font) {
	if (defaultFont) {
		Logger::error_and_throw("Default font should be set once");
	}
	defaultFont = &font;
}