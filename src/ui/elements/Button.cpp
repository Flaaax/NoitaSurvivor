#include "Button.h"

#include "../Window.h"
#include "../render/Painter.h"
#include "RichText.h"
#include "src/ui/layout/BoxLayout.h"

#include <SFML/Graphics/RectangleShape.hpp>

namespace flx::ui {
	Button::Button(rect geometry) {
		setFrame(geometry);

		sizePolicy = Fixed;

		auto layout = std::make_unique<VBoxLayout>();
		layout->widthPolicy = Layout::Fill;
		layout->heightPolicy = Layout::Fill;
		layout->alignY = Layout::Center;
		layout->alignX = Layout::Center;

		auto text = std::make_unique<RichText>();
		this->text = text.get();

		layout->add(text | flx::move);

		setLayout(layout | flx::move);
	}

	std::optional<EventResult> Button::handleEvent(const UIEvent& event) {
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
				return EventResult{this, EventResult::Pressed{}};
			}
		} else if (rawEvent.is<sf::Event::MouseButtonReleased>()) {
			if (frame.contains(event.localCtx.mouseLocal)) {
				state = Hovered;
				if (isActuallyPressed) {
					if (onClick) {
						onClick();
					}
					isActuallyPressed = false;
					return EventResult{this, EventResult::Clicked{}};
				}
			}
			state = Normal;
			isActuallyPressed = false;
		}
		return {};
	}

	void Button::draw(const UIPainter& canvas) const {
		if (hasBackground) {
			sf::RectangleShape shape;
			shape.setSize(getSize());
			if (state == Normal) {
				shape.setFillColor({190, 190, 190});
			} else if (state == Hovered) {
				shape.setFillColor({210, 210, 210});
			} else {
				shape.setFillColor({195, 195, 195});
			}
			shape.setOutlineThickness(3.f);
			shape.setOutlineColor({150, 150, 150});
			canvas.draw(shape);
		}

		Panel::draw(canvas);
	}

	void Button::setText(std::string text, u32 characterSize) {
		this->text->setString(text | move);
		this->text->setCharacterSize(characterSize);
		Panel::refresh();
	}
} // namespace flx::ui