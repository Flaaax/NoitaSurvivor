#include "MaterialBar.h"
#include "../../app/global/LocManager.h"
#include "src/ui/elements/NRichText.h"
#include "src/ui/global/NGlobal.h"
#include "src/ui/render/NPainter.h"
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
	text.setOutlineColor({0, 0, 0});
	text.setOutlineThickness(1.5f);

	if (format.empty()) {
		const auto materialString = LocManager::inst().debugGetString("ui", "material", true);
		format = "{}" + materialString;
	}

	updateText();
}

void MaterialBar::draw(const NPainter& canvas) const {
	canvas.draw(text);
}

void MaterialBar::updateText() {
	text.setString(Util::format(format, material));
	auto layout = text.getLayoutSize();
	frame = {{}, layout};
	frame.setRightTop(rightTop);
}
