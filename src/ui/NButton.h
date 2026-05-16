#pragma once

#include "./NObject.h"
#include "src/utils/NString.h"
#include <functional>

class NButton : public NObject {
public:
	mutable sf::Text text;
	std::function<void()> onClick;

	bool isButtonVisible = true;

	enum ButtonState {
		Hovered,
		Normal,
		Pressed
	} state = Normal;

	NButton() : NButton({0, 0, 50, 50}) {
	}

	explicit NButton(nrect geometry);

	bool handleEvent(const sf::Event& event) override;
	void draw(Renderer& renderer) const override;

	sf::Text& getText() const;
	void setText(const NString& text, unsigned int characterSize = 30) const;

	void setOnClick(const std::function<void()>& func) {
		onClick = func;
	}
};