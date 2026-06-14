#pragma once

namespace sf {
	class Sprite;
}

namespace flx::app {
	class SpriteMgr {
	public:
		static const sf::Sprite& getSprite(std::string_view name);
	};
} // namespace flx::game