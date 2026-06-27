#pragma once
#include "src/ui/Object.h"
#include "src/ui/shapes/RichTextShape.h"

namespace flx::ui {
	class RichText : public Object {
	private:
		RichTextShape text;
		u32 designedPx;

	public:
		bool alignCenter{};
		bool useRealPx = true;

		// bool fixedPosition{};

		explicit RichText(const sf::Font& font, std::string utf8Markup = {}, u32 characterSize = 30u, float lineSpacing = 1.f);
		explicit RichText(std::string utf8Markup = {}, u32 characterSize = 30u, float lineSpacing = 1.f);
		explicit RichText(std::string markup, const RichTextShape::Preset& preset);
		static auto makePreset();
		void draw(const UIPainter& canvas) const override;
		Measure measure(LayoutConstraint constraint) override;
		void arrange(rect rect) override;
		void refresh() override;
		void setString(std::string markup);
		void setCharacterSize(u32 designedPx);
	};
} // namespace flx::ui