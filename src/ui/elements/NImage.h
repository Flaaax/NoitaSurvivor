#pragma once
#include "src/ui/NObject.h"

#include <SFML/Graphics/Sprite.hpp>

class NImage : public NObject {
protected:
	mutable sf::Sprite sprite;

public:
	explicit NImage(const sf::Texture& texture);

	void draw(const NCanvas& canvas) const override;
};