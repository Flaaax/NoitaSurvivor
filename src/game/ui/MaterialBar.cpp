#include "MaterialBar.h"
#include "../../app/global/LocManager.h"
#include "src/ui/elements/NRichText.h"
#include "src/ui/global/Global.h"
#include "src/ui/render/NPainter.h"
#include "src/utils/Text/Format.h"

namespace flx::ui {
	MaterialBar::MaterialBar(vec2 rightTop, float height)
		: rightTop(rightTop), height(height), text(ui::Global::getDefaultFont()) {
		frame = {{}, {500.f, height}};
		frame.setRightTop(rightTop);

		auto style = text.getDefaultStyle();
		style.color = {240, 240, 240};
		style.effects.add(text::TextEffect::Bold);
		text.setDefaultStyle(style);
		text.setCharacterSize(static_cast<u32>(this->height * 0.9f));
		text.setOutlineColor({0, 0, 0});
		text.setOutlineThickness(1.5f);

		if (format.empty()) {
			const auto materialString = app::LocManager::inst().debugGetString("ui", "material", true);
			format = "{}" + materialString;
		}

		updateText();
	}

	void MaterialBar::draw(const ui::NUIPainter& canvas) const {
		canvas.draw(text);
	}

	void MaterialBar::updateText() {
		text.setString(flx::vformat(format, material));
		auto layout = text.getLayoutSize();
		frame = {{}, layout};
		frame.setRightTop(rightTop);
	}
} // namespace flx::game