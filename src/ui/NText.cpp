#include "NText.h"
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

void NText::draw(Renderer& renderer) const {
	if (align == LeftTop) {
		sfText.setPosition(this->getPosition());
		renderer.drawGui(sfText);
	} else if (align == Center) {
		nrect bounds = sfText.getLocalBounds();
		bounds.setCenter(m_geometry.center());
		sfText.setPosition(bounds.position);
		renderer.drawGui(sfText);
	}
}

void NLineText::draw(Renderer& renderer) const {
	NString text;
	for (size_t i = 0; i < std::min(lineLimit, strings.size()); i++) {
		if (i != 0)
			text += '\n';
		text += strings[i];
	}
	sfText.setString(text);
	sfText.setPosition(this->getPosition());
	renderer.drawGui(sfText);
}
