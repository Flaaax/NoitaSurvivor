#pragma once
#include "../utils/Container/Container.h"

namespace sf { class Sprite; }

class SpriteMgr {
public:
	static const sf::Sprite& getSprite(std::string_view name);
};