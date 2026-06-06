#pragma once
#include "src/ui/layout/NLayout.h"
#include "src/ui/widget/NWidget.h"

#include <SFML/Graphics/Color.hpp>

class NPanel : public NWidget {
protected:
	NLayout* getLayout() const;

public:
	enum Policy {
		ExpandBottom,
		Auto,
		Fixed
	};

	Policy policy = ExpandBottom; // Not supported
	Policy sizePolicy = Auto;

	sf::Color backgroundColor = sf::Color::Transparent;
	sf::Color outlineColor = {120, 120, 120};
	float outlineThickness = 2.f;

	void draw(const NPainter& canvas) const override;
	void setLayout(n_unique<NLayout> layout);
	void refreshLayout();
};