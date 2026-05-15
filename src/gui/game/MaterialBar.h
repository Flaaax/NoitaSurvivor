#pragma once

#include "../NObject.h"
#include <SFML/Graphics/Text.hpp>


class MaterialBar :public NObject {
private:
	int material = 0;
	nvec2 topRight;
	float height;
	mutable sf::Text text;

public:
	MaterialBar(nvec2 topRight, float height);

	void setData(int material) {
		this->material = material;
		updateText();
	}

	void updateText();

	void draw(Renderer& renderer)const override;
};