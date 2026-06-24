#pragma once
#include "src/ui/Object.h"
#include "src/ui/global/Global.h"
#include "src/ui/render/Painter.h"
#include "src/ui/shapes/RichTextShape.h"

namespace flx::ui {
	class RichText : public Object {
	public:
		RichTextShape text;
		u32 designedPx;
		bool alignCenter{};
		bool useRealPx = true;

		// bool fixedPosition{};

		explicit RichText(const sf::Font& font, std::string_view utf8Markup = {}, u32 characterSize = 30u)
			: text(font, utf8Markup, characterSize), designedPx(characterSize) {}

		explicit RichText(std::string_view utf8Markup = {}, u32 characterSize = 30u)
			: RichText(Global::getDefaultFont(), utf8Markup, characterSize) {}

		void draw(const UIPainter& canvas) const override {
			if (useRealPx) {
				canvas.drawText(text);
			} else {
				canvas.draw(text);
			}
		}

		LayoutResult measure(LayoutConstraint constraint) override {
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

		void arrange(rect rect) override {
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
} // namespace flx::ui