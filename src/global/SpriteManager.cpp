#include "SpriteManager.h"
#include "AssetManager.h"
#include "DataManager.h"
#include <SFML/Graphics.hpp>

class SpriteMgrImpl {
	N_DECL_SINGLETON(SpriteMgrImpl);
	void initSprites();

public:
	Util::StdMap<sf::Sprite> sprites;
	SpriteMgrImpl() {
		initSprites();
	}
};

static SpriteMgrImpl& inst() {
	return SpriteMgrImpl::inst();
}

void SpriteMgrImpl::initSprites() {
	for (auto& [name, data] : DataMgr::getSpriteData()) {
		auto [it, state] = sprites.emplace(std::piecewise_construct,
										   std::forward_as_tuple(name),
										   std::forward_as_tuple(AssetMgr::getSpriteTexture(data.texture)));
		if (!state) {
			Logger::warn("Sprite name {} is dulplicated! Skipped loading.", name);
			continue;
		}
		auto& sprite = it->second;
		if (data.centerAligned) {
			sprite.setOrigin(sprite.getLocalBounds().size / 2.f);
		}
		sprite.setScale(data.scale);
	}
}

const sf::Sprite& SpriteMgr::getSprite(std::string_view name) {
	if (const auto sprite = inst().sprites.try_find(name)) {
		return *sprite;
	}
	Logger::error_and_throw("Cannot find sprite: {}", name);
}
