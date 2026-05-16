#include "MaterialBar.h"
#include "src/global/AssetManager.h"
#include <src/utils/NString.h>

MaterialBar::MaterialBar(nvec2 topRight, float height) : topRight(topRight), height(height), text(AssetMgr::getDefaultFont()) {
	text.setFillColor({240, 240, 240});
	text.setOutlineColor({0, 0, 0});
	text.setCharacterSize(static_cast<unsigned int>(this->height * 0.9));

	text.setOutlineThickness(1.f);
	text.setStyle(sf::Text::Bold);

	updateText();
}

void MaterialBar::updateText() {
	text.setString(NString("{}材料", material));
}

void MaterialBar::draw(Renderer& renderer) const {
	nrect textRect = text.getGlobalBounds();
	textRect.setTopRight(topRight);
	text.setPosition(textRect.position);
	renderer.drawGui(text);
}
