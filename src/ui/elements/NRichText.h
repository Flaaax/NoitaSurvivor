#pragma once
#include "src/ui/NObject.h"
#include "src/ui/global/NGlobal.h"
#include "src/ui/render/NCanvas.h"
#include "src/ui/shapes/NRichTextShape.h"

class NRichText : public NObject {
public:
	NRichTextShape text;

	bool alignCenter{};
	//bool fixedPosition{};

	explicit NRichText(const sf::Font& font, std::string_view utf8Markup = {}, u32 characterSize = 30u)
		: text(font, utf8Markup, characterSize) {}

	explicit NRichText(std::string_view utf8Markup = {}, u32 characterSize = 30u)
		: NRichText(NGlobal::getDefaultFont(), utf8Markup, characterSize) {}

	void draw(const NCanvas& canvas) const override {
		canvas.draw(text);
	}

	NLayoutResult measure(NLayoutConstraint constraint) override {
		text.setLineWidth(constraint.maxSize.x);
		if (alignCenter) {
			return {.size = constraint.maxSize};
		}
		// setSize(text.getLayoutSize());
		return {.size = text.getLayoutSize()};
	}

	void arrange(nrect rect) override {
		text.setLineWidth(rect.size.x);
		setPosition(rect.position);
		setSize(text.getLayoutSize());
		if (alignCenter) {
			auto layout = text.getGlobalLayout();
			layout.setCenter(frame.center());
			text.setPosition(layout.position);
		}
		visualDirty = false;
	}
};
