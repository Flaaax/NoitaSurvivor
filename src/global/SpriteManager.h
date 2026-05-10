#pragma once
#include"src/utils/Container.h"
#include"src/utils/Singleton.h"


namespace sf { class Sprite; }

class SpriteMgr {
public:
	static sf::Sprite* getSprite(std::string_view name);
};