
#include "NButton.h"
#include "./Renderer.h"
#include "NWindow.h"
#include "src/global/AssetManager.h"

NButton::NButton(nrect geometry) : text(AssetMgr::getDefaultFont()) {
	m_geometry = geometry;
	text.setFillColor({0, 0, 0});
}
bool NButton::handleEvent(const sf::Event& event) {
	if (event.is<sf::Event::MouseMoved>()) {
		if (m_geometry.contains(NWindow::mouseRenderPos)) {
			if (state != Pressed) {
				state = Hovered;
			}
		} else {
			state = Normal;
		}
	} else if (event.is<sf::Event::MouseButtonPressed>()) {
		if (m_geometry.contains(NWindow::mouseRenderPos)) {
			state = Pressed;
			return true;
		}
	} else if (event.is<sf::Event::MouseButtonReleased>()) {
		if (m_geometry.contains(NWindow::mouseRenderPos)) {
			state = Hovered;
			if (onClick) {
				onClick();
			}
			return true;
		} else {
			state = Normal;
		}
	}
	return false;
}

void NButton::draw(Renderer& renderer) const {
	if (isButtonVisible) {
		sf::RectangleShape shape;
		shape.setPosition(m_geometry.position);
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
		renderer.drawGui(shape);
	}

	if (!text.getString().isEmpty()) {
		nrect bounds = text.getGlobalBounds();
		bounds.setCenter(m_geometry.center());
		text.setPosition(bounds.position);
		renderer.drawGui(text);
	}
}

sf::Text& NButton::getText() const {
	return text;
}

void NButton::setText(const NString& text, unsigned int characterSize) const {
	this->text.setString(text);
	this->text.setCharacterSize(characterSize);
}
