#include "NTooltip.h"

#include "render/NPainter.h"

void NTooltip::updateLayout() {
	nvec2 layout{};
	bool skipFirstText = false;
	constexpr float fixedLineSpacing = 4.f;
	if (sprite) {
		const nrect bounds = sprite->getGlobalBounds();
		layout += bounds.size;
		if (!textLines.empty() && hasTitle) {
			auto& title = textLines.front();
			title.setPosition({layout.x, 0});
			const nvec2 titleLayout = title.getLayoutSize();
			layout.x += titleLayout.x;
			layout.y = std::max(layout.y, titleLayout.y);
			skipFirstText = true;
		}
		layout.y += fixedLineSpacing;
	}
	for (auto& text : textLines) {
		if (skipFirstText) {
			skipFirstText = false;
			continue;
		}
		text.setPosition({0, layout.y});
		const nvec2 textLayout = text.getLayoutSize();
		layout.x = std::max(layout.x, textLayout.x);
		layout.y += textLayout.y + fixedLineSpacing;
	}
	frame.size = layout;
}

// void NTooltip::setContent(const NTooltipSpec& spec, const NStyle& style) {
// 	if (spec.iconTexture) {
// 		sprite.emplace(*spec.iconTexture);
// 		const nvec2 textureSize = static_cast<nvec2>(sprite->getTexture().getSize());
// 		const nvec2 scale = spec.iconSize / textureSize;
// 		sprite->setScale(scale);
// 	} else {
// 		sprite.reset();
// 	}
//
// 	textLines.clear();
// 	hasTitle = false;
//
// 	if (!spec.title.empty()) {
// 		hasTitle = true;
// 		textLines.emplace_back(*style.font, spec.title, style.titleSize);
// 	}
//
// 	for (const u64 i : spec.contents.indices()) {
// 		const auto& text = spec.contents[i];
// 		textLines.emplace_back(*style.font, text, style.contentSize);
// 	}
//
// 	if (!spec.flavor.empty()) {
// 		textLines.emplace_back(*style.font, spec.flavor, style.flavorSize);
// 	}
//
// 	updateLayout();
// }

void NTooltip::draw(const NPainter& canvas) const {
	sf::RectangleShape background;
	background.setSize(frame.size);
	background.setFillColor({150, 150, 150});
	background.setOutlineColor({130, 130, 130});
	background.setOutlineThickness(2);
	canvas.draw(background);
	if (sprite) {
		//sprite->setPosition(getPosition());
		canvas.draw(*sprite);
	}
	for (const auto& text : textLines) {
		canvas.draw(text);
	}
}