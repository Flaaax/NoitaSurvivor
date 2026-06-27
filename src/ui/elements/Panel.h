#pragma once
#include "src/ui/context/Policy.h"
#include "src/ui/layout/Layout.h"
#include "src/ui/widget/Widget.h"

#include <SFML/Graphics/Color.hpp>

namespace flx::ui {
	class Panel : public Widget {
	protected:
		Ref<Layout> layout{};

	public:
		struct Def {
			Policy sizePolicy = Policy::Shrink; // Shrink, Fixed
			sf::Color backgroundColor = sf::Color::Transparent;
			sf::Color outlineColor = {120, 120, 120};
			float outlineThickness = 0.f;
			vec2 size = {50.f, 50.f};
		};

		Policy sizePolicy = Policy::Shrink; // Shrink, Fixed

		sf::Color backgroundColor = sf::Color::Transparent;
		sf::Color outlineColor = {120, 120, 120};
		float outlineThickness = 0.f;

		void draw(const UIPainter& canvas) const override;
		void setLayout(SUnique<Layout> layout);
		void refresh() override;
		Measure measure(LayoutConstraint constraint) override;
		void arrange(rect rect) override;
	};
} // namespace flx::ui