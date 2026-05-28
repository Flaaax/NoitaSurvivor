
#include "NButton.h"
#include "../NWindow.h"
#include "../render/NCanvas.h"
#include "src/global/AssetManager.h"

#include <SFML/Graphics/RectangleShape.hpp>

NButton::NButton(nrect geometry) : text(AssetMgr::getDefaultFont()) {
	this->frame = geometry;
	text.setFillColor({0, 0, 0});
}

std::optional<NEventResult> NButton::handleEvent(const NUIEvent& event) {
	auto& rawEvent = event.ctx.rawEvent;
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
			return NEventResult{this, NEventResult::Pressed{}};
		}
	} else if (rawEvent.is<sf::Event::MouseButtonReleased>()) {
		if (frame.contains(event.localCtx.mouseLocal)) {
			state = Hovered;
			if (onClick) {
				onClick();
			}
			return NEventResult{this, NEventResult::Clicked{}};
		}
		state = Normal;
	}
	return {};
}

void NButton::draw(const NCanvas& canvas) const {
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

	if (!text.getString().isEmpty()) {
		nrect bounds = text.getGlobalBounds();
		bounds.setCenter(getLocalBounds().center());
		text.setPosition(bounds.position);
		canvas.draw(text);
	}
}

sf::Text& NButton::getText() const {
	return text;
}

void NButton::setText(const NString& text, unsigned int characterSize) const {
	this->text.setString(text);
	this->text.setCharacterSize(characterSize);
}
