#include "NButton.h"

#include "../NWindow.h"
#include "../render/NPainter.h"
#include "NRichText.h"
#include "src/ui/layout/NBoxLayout.h"

#include <SFML/Graphics/RectangleShape.hpp>

NButton::NButton(nrect geometry) {
	setFrame(geometry);

	sizePolicy = Fixed;

	auto layout = std::make_unique<NVBoxLayout>();
	layout->widthPolicy = NVBoxLayout::Fill;
	layout->alignY = NVBoxLayout::Center;
	layout->alignX = NVBoxLayout::Center;

	auto text = std::make_unique<NRichText>();
	this->text = text.get();

	layout->add(text | Util::move);

	setLayout(layout | Util::move);
}

std::optional<NEventResult> NButton::handleEvent(const NUIEvent& event) {
	auto& rawEvent = event.windowEvent.rawEvent;
	if (rawEvent.is<sf::Event::MouseMoved>()) {
		if (frame.contains(event.localCtx.mouseLocal)) {
			if (state != Pressed) {
				state = Hovered;
			}
		} else {
			state = Normal;
		}
	} else if (rawEvent.is<sf::Event::MouseButtonPressed>()) {
		if (frame.contains(event.localCtx.mouseLocal)) {
			state = Pressed;
			isActuallyPressed = true;
			return NEventResult{this, NEventResult::Pressed{}};
		}
	} else if (rawEvent.is<sf::Event::MouseButtonReleased>()) {
		if (frame.contains(event.localCtx.mouseLocal)) {
			state = Hovered;
			if (isActuallyPressed) {
				if (onClick) {
					onClick();
				}
				isActuallyPressed = false;
				return NEventResult{this, NEventResult::Clicked{}};
			}
		}
		state = Normal;
		isActuallyPressed = false;
	}
	return {};
}

void NButton::draw(const NUIPainter& canvas) const {
	if (isButtonVisible) {
		sf::RectangleShape shape;
		shape.setSize(getSize());
		if (state == Normal) {
			shape.setFillColor(sf::Color(190, 190, 190));
		} else if (state == Hovered) {
			shape.setFillColor(sf::Color(210, 210, 210));
		} else {
			shape.setFillColor(sf::Color(195, 195, 195));
		}
		shape.setOutlineThickness(3.f);
		shape.setOutlineColor({150, 150, 150});
		canvas.draw(shape);
	}

	NPanel::draw(canvas);
}

void NButton::setText(std::string_view text, u32 characterSize) {
	this->text->text.setString(text);
	this->text->text.setCharacterSize(characterSize);
	NPanel::refresh();
}
