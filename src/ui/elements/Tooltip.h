#pragma once
#include "../shapes/RichTextShape.h"
#include "../widget/Widget.h"

namespace flx::ui {
	class Tooltip : public Object {
	private:
		mutable std::optional<sf::Sprite> sprite{};
		mutable Vector<RichTextShape> textLines{};
		bool hasTitle{};

		void updateLayout();

	public:
		// void setContent(const NTooltipSpec& spec, const NStyle& style);
		void draw(const UIPainter& canvas) const override;
	};
} // namespace flx::ui