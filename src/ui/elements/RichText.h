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

		explicit RichText(const sf::Font& font, std::string utf8Markup = {}, u32 characterSize = 30u, float lineSpacing = 1.f)
			: text(font, std::move(utf8Markup), characterSize, lineSpacing), designedPx(characterSize) {}

		explicit RichText(std::string utf8Markup = {}, u32 characterSize = 30u, float lineSpacing = 1.f)
			: RichText(Global::getDefaultFont(), std::move(utf8Markup), characterSize) {}

		explicit RichText(std::string markup, const RichTextShape::Preset& preset)
			: text(std::move(markup), preset), designedPx(preset.characterSize) {}

		static auto makePreset() {
			return RichTextShape::Preset{
				.font = Global::getDefaultFont(),
			};
		}

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
				setFrame(rect);
				//setPosition(rect.position);
				//setSize(text.getLayoutSize());

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
				//setPosition(rect.position);
				//setSize(rect.size / viewport.scale);
				setFrame(rect);

				if (alignCenter) {
					auto layout = text.getGlobalLayout() << viewport;
					layout.setCenter(frame.center());
					text.setPosition(layout.position >> viewport);
				}
				visualDirty = false;
			}
		}

		void refresh() override {
			// if (getRoot()) {
			// 	auto& viewport = getGlobalViewport();
			// 	text.setCharacterSize(viewport.getPx(designedPx));
			// }
			arrange(getFrame());
		}
	};
} // namespace flx::ui