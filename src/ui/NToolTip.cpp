#include "NToolTip.h"

#include "render/NCanvas.h"

void NToolTip::updateLayout() {
	nvec2 layout{};
	bool skipFirstText = false;
	if (sprite) {
		const nrect bounds = sprite->getGlobalBounds();
		sprite->setPosition(-bounds.position);
		layout += bounds.size;
		if (!textLines.empty()) {
			auto& title = textLines.front();
			title.setPosition({layout.y, 0});
			const nvec2 titleLayout = title.getLayoutSize();
			layout.x += titleLayout.x;
			layout.y = std::max(layout.y, titleLayout.y);
			skipFirstText = true;
		}
	}
	for (auto& text : textLines) {
		if (skipFirstText) {
			skipFirstText = false;
			continue;
		}
		text.setPosition(layout);
		layout += text.getLayoutSize();
	}
	geometry.size = layout;
}

void NToolTip::set(Util::Vector<NRichTextShape> textLines, n_unique<sf::Sprite> sprite) {
	this->sprite = std::move(sprite);
	this->textLines = std::move(textLines);
	updateLayout();
}

void NToolTip::draw(const NCanvas& canvas) const {
	sf::RectangleShape background;
	background.setSize(geometry.size);
	background.setFillColor({100, 100, 100});
	background.setOutlineColor({150, 150, 150});
	canvas.draw(background);
	if (sprite) {
		canvas.draw(*sprite);
	}
	for (const auto& text : textLines) {
		canvas.draw(text);
	}
}