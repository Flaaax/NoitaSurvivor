#pragma once

#include "src/ui/NObject.h"

#include <SFML/Graphics/Text.hpp>


class MaterialBar :public NObject {
private:
	int material = 0;
	nvec2 rightTop;
	float height;
	mutable sf::Text text;

public:
	MaterialBar(nvec2 rightTop, float height);

	void setData(int material) {
		this->material = material;
		updateText();
	}

	void updateText();

	void draw(const NCanvas& canvas) const override;
};