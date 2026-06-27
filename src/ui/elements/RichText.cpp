#include "RichText.h"

#include "src/ui/global/Global.h"
#include "src/ui/render/Painter.h"
#include "src/ui/render/Viewport.h"

namespace flx::ui {

	RichText::RichText(const sf::Font& font, std::string utf8Markup, u32 characterSize, float lineSpacing)
		: text(font, std::move(utf8Markup), characterSize, lineSpacing), designedPx(characterSize) {}

	RichText::RichText(std::string utf8Markup, u32 characterSize, float lineSpacing)
		: RichText(Global::getDefaultFont(), std::move(utf8Markup), characterSize, lineSpacing) {}

	RichText::RichText(std::string markup, const RichTextShape::Preset& preset)
		: text(std::move(markup), preset), designedPx(preset.characterSize) {}

	auto RichText::makePreset() {
		return RichTextShape::Preset{
			.font = Global::getDefaultFont(),
		};
	}

	void RichText::draw(const UIPainter& canvas) const {
		if (useRealPx) {
			canvas.drawText(text);
		} else {
			canvas.draw(text);
		}
	}

	Measure RichText::measure(LayoutConstraint constraint) {
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

	void RichText::arrange(rect rect) {
		if (!getRoot() || !useRealPx) {
			text.setCharacterSize(designedPx);
			text.setLineWidth(rect.size.x);
			setFrame(rect);
			// setPosition(rect.position);
			// setSize(text.getLayoutSize());

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
			// setPosition(rect.position);
			// setSize(rect.size / viewport.scale);
			setFrame(rect);

			if (alignCenter) {
				auto layout = text.getGlobalLayout() << viewport;
				layout.setCenter(frame.center());
				text.setPosition(layout.position >> viewport);
			}
			visualDirty = false;
		}
	}

	void RichText::refresh() {
		// if (getRoot()) {
		// 	auto& viewport = getGlobalViewport();
		// 	text.setCharacterSize(viewport.getPx(designedPx));
		// }
		arrange(getFrame());
		visualDirty = false;
	}

	void RichText::setString(std::string markup) {
		text.setString(markup | move);
	}

	void RichText::setCharacterSize(u32 designedPx) {
		this->designedPx = designedPx;
		refresh();
	}

} // namespace flx::ui