#pragma once
#include "src/ui/layout/NLayout.h"
#include "src/ui/widget/NWidget.h"

#include <SFML/Graphics/Color.hpp>

class NPanel : public NWidget {
protected:
	NObject* getLayout() const;

public:
	enum Policy {
		ExpandBottom
	};

	Policy policy = ExpandBottom; // Not supported

	sf::Color backgroundColor = {160, 160, 160};
	sf::Color outlineColor = {120, 120, 120};
	float outlineThickness = 2.f;

	void draw(const NCanvas& canvas) const override;
	void setLayout(n_unique<NLayout> layout);
};