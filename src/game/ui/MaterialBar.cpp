#include "MaterialBar.h"
#include "src/global/AssetManager.h"
#include "src/ui/render/NCanvas.h"
#include "src/utils/Text/NString.h"

MaterialBar::MaterialBar(nvec2 rightTop, float height) : rightTop(rightTop), height(height), text(AssetMgr::getDefaultFont()) {
	text.setFillColor({240, 240, 240});
	text.setOutlineColor({0, 0, 0});
	text.setCharacterSize(static_cast<unsigned int>(this->height * 0.9));

	text.setOutlineThickness(1.f);
	text.setStyle(sf::Text::Bold);

	updateText();
}

void MaterialBar::updateText() {
	text.setString(NString("{}材料", material));
	frame = text.getLocalBounds();
	frame.setRightTop(rightTop);
}

void MaterialBar::draw(const NCanvas& canvas) const {
	canvas.draw(text);
}
