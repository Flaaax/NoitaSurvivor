#pragma once
#include "src/ui/layout/Layout.h"
#include "src/ui/widget/Widget.h"

#include <SFML/Graphics/Color.hpp>

namespace flx::ui {
	class Panel : public Widget {
	protected:
		Layout* getLayout() const;

	public:
		enum Policy {
			ExpandBottom,
			Auto,
			Fixed
		};

		// Policy policy = ExpandBottom; // Not supported
		Policy sizePolicy = Auto;

		sf::Color backgroundColor = sf::Color::Transparent;
		sf::Color outlineColor = {120, 120, 120};
		float outlineThickness = 0.f;

		void draw(const UIPainter& canvas) const override;
		void setLayout(Unique<Layout> layout);
		void refresh() override;
	};
} // namespace flx::ui