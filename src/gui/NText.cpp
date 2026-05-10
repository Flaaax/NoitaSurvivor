#include"NText.h"
#include"src/global/AssetManager.h"


NText::NText() :NText("") {
}

NText::NText(const NString& str, int align, unsigned int characterSize):align(align) {
	sfText.setString(str);
	sfText.setCharacterSize(characterSize);
	setDefaultFont();
	sfText.setFillColor({});
}

void NText::draw(Renderer& renderer) const {
	if (align == LeftTop) {
		sfText.setPosition(this->getPosition());
		renderer.drawGui(sfText);
	}
	else if (align == Center) {
		nrect bounds = sfText.getLocalBounds();
		bounds.setCenter(m_geometry.center());
		sfText.setPosition(bounds.position);
		renderer.drawGui(sfText);
	}
}

void NText::setDefaultFont() {
	sfText.setFont(AssetMgr::getDefaultFont());
}

void NLineText::draw(Renderer& renderer) const {
	NString text;
	for (size_t i = 0; i < std::min(lineLimit, strings.size()); i++) {
		if (i != 0)text += '\n';
		text += strings[i];
	}
	sfText.setString(text);
	sfText.setPosition(this->getPosition());
	renderer.drawGui(sfText);
}
