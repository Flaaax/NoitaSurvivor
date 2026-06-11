#pragma once
#include "src/ui/NObject.h"
#include "src/ui/global/NGlobal.h"
#include "src/ui/render/NPainter.h"
#include "src/ui/shapes/NRichTextShape.h"

class NRichText : public NObject {
public:
	NRichTextShape text;
	u32 designedPx;
	bool alignCenter{};
	bool useRealPx = true;

	// bool fixedPosition{};

	explicit NRichText(const sf::Font& font, std::string_view utf8Markup = {}, u32 characterSize = 30u)
		: text(font, utf8Markup, characterSize), designedPx(characterSize) {}

	explicit NRichText(std::string_view utf8Markup = {}, u32 characterSize = 30u)
		: NRichText(NGlobal::getDefaultFont(), utf8Markup, characterSize) {}

	void draw(const NUIPainter& canvas) const override {
		if (useRealPx) {
			canvas.drawText(text);
		} else {
			canvas.draw(text);
		}
	}

	NLayoutResult measure(NLayoutConstraint constraint) override {
		if (!getRoot() || !useRealPx) {
			text.setCharacterSize(designedPx);
			text.setLineWidth(constraint.maxSize.x);

			if (alignCenter) {
				return {.size = constraint.maxSize};
			}
			// setSize(text.getLayoutSize());
			return {.size = text.getLayoutSize()};
		}

		auto& viewport = getGlobalViewport();
		text.setCharacterSize(viewport.getPx(designedPx));
		text.setLineWidth(constraint.maxSize.x * viewport.scale.x);

		if (alignCenter) {
			return {.size = constraint.maxSize};
		}
		// setSize(text.getLayoutSize());
		return {.size = text.getLayoutSize() / viewport.scale};
	}

	void arrange(nrect rect) override {
		if (!getRoot() || !useRealPx) {
			text.setCharacterSize(designedPx);
			text.setLineWidth(rect.size.x);
			setPosition(rect.position);
			setSize(text.getLayoutSize());

			if (alignCenter) {
				auto layout = text.getGlobalLayout();
				layout.setCenter(frame.center());
				text.setPosition(layout.position);
			}
			visualDirty = false;
		} else {
			auto& viewport = getGlobalViewport();
			text.setCharacterSize(viewport.getPx(designedPx));
			text.setLineWidth(rect.size.x * viewport.scale.x);
			setPosition(rect.position);
			setSize(text.getLayoutSize() / viewport.scale);

			if (alignCenter) {
				auto layout = text.getGlobalLayout() << viewport;
				layout.setCenter(frame.center());
				text.setPosition(layout.position >> viewport);
			}
			visualDirty = false;
		}
	}

	void refresh() override {
		if (getRoot()) {
			auto& viewport = getGlobalViewport();
			text.setCharacterSize(viewport.getPx(designedPx));
		}
	}
};
