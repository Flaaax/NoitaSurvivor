#include "NText.h"

#include "render/NCanvas.h"
#include "src/global/AssetManager.h"

NText::NText() : NText("") {
}

NText::NText(const NString& str, AlignOption align, unsigned int characterSize)
	: sfText(AssetMgr::getDefaultFont()),
	  align(align) {
	sfText.setString(str);
	sfText.setCharacterSize(characterSize);
	sfText.setFillColor({});
}
NText::NText(const NString& str, const sf::Font& font, AlignOption align, unsigned int characterSize)
	: sfText(font, str, characterSize),
	  align(align) {
	sfText.setFillColor({});
}

NText::NText(const sf::Text& text, AlignOption align) : sfText(text), align(align) {
}

void NText::draw(const NCanvas& canvas) const {
	if (align == LeftTop) {
		//sfText.setPosition(this->getPosition());
		canvas.draw(sfText);
	} else if (align == Center) {
		nrect bounds = sfText.getLocalBounds();
		bounds.setCenter(getLocalBounds().center());
		sfText.setPosition(bounds.position);
		canvas.draw(sfText);
	}
}

void NLineText::draw(const NCanvas& canvas) const {
	NString text;
	for (size_t i = 0; i < std::min(lineLimit, strings.size()); i++) {
		if (i != 0)
			text += '\n';
		text += strings[i];
	}
	sfText.setString(text);
	canvas.draw(sfText);
}
