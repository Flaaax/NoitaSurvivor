#pragma once
#include "src/ui/NObject.h"
#include "src/ui/render/NCanvas.h"
#include "src/ui/shapes/NRichTextShape.h"

class NRichText : public NObject {
protected:
	NRichTextShape text;

public:
	explicit NRichText(const sf::Font& font, std::string_view utf8Markup = {}, u32 characterSize = 30u)
		: text(font, utf8Markup, characterSize) {}

	void draw(const NCanvas& canvas) const override {
		canvas.draw(text);
	}

	NLayoutResult measure(NLayoutConstraint constraint) override {
		text.setLineWidth(constraint.maxSize.x);
		setSize(text.getLayoutSize());
		return {.size = getSize()};
	}

	void arrange(nrect rect) override {
		setPosition(rect.position);
		text.setLineWidth(rect.size.x);
	}
};
