#include "MaterialBar.h"
#include "src/global/LocManager.h"
#include "src/ui/elements/NRichText.h"
#include "src/ui/global/NGlobal.h"
#include "src/ui/render/NCanvas.h"
#include "src/utils/Text/Format.h"
#include "src/utils/Text/NString.h"

MaterialBar::MaterialBar(nvec2 rightTop, float height)
	: rightTop(rightTop), height(height), text(NGlobal::getDefaultFont()) {
	frame = {{}, {500.f, height}};
	frame.setRightTop(rightTop);

	auto style = text.getDefaultStyle();
	style.color = {240, 240, 240};
	style.effects.add(Util::TextEffect::Bold);
	text.setDefaultStyle(style);
	text.setCharacterSize(static_cast<u32>(this->height * 0.9f));

	if (format.empty()) {
		const auto materialString = LocManager::inst().debugGetString("ui", "material", true);
		format = "{}" + materialString;
	}

	auto text = std::make_unique<NRichText>(NGlobal::getDefaultFont(), "", 30);

	// text.setFillColor({240, 240, 240});
	// text.setOutlineColor({0, 0, 0});
	// text.setCharacterSize();
	//
	// text.setOutlineThickness(1.f);
	// text.setStyle(sf::Text::Bold);

	updateText();
}

void MaterialBar::draw(const NCanvas& canvas) const {
	canvas.draw(text);
}

void MaterialBar::updateText() {
	text.setString(Util::format(format, material));
	auto layout = text.getLayoutSize();
	frame = {{}, layout};
	frame.setRightTop(rightTop);
}
