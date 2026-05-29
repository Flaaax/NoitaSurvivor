#include "NGlobalVariables.h"

#include "src/utils/Logger.h"

const sf::Font& NGlobalVariables::getDefaultFont() {
	if (!defaultFont) {
		Logger::error_and_throw("No default font avaliable");
	}
	return *defaultFont;
}

void NGlobalVariables::setDefaultFont(const sf::Font& font) {
	if (defaultFont) {
		Logger::error_and_throw("Default font should be set once");
	}
	defaultFont = &font;
}