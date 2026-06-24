#pragma once
#include "../context/NStyle.h"
#include "../shapes/NRichTextShape.h"
#include "../widget/NWidget.h"

namespace flx::ui {
	class NTooltip : public Object {
	private:
		mutable std::optional<sf::Sprite> sprite{};
		mutable flx::Vector<NRichTextShape> textLines{};
		bool hasTitle{};

		void updateLayout();

	public:
		// void setContent(const NTooltipSpec& spec, const NStyle& style);
		void draw(const UIPainter& canvas) const override;
	};
} // namespace flx::ui