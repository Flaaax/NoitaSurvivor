#include"SpriteManager.h"
#include"DataManager.h"
#include"AssetManager.h"
#include<SFML/Graphics.hpp>

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
		auto& sprite = sprites[name];
		sprite.setTexture(AssetMgr::getSpriteTexture(data.texture));
		if (data.centerAligned) {
			sprite.setOrigin(sprite.getLocalBounds().getSize() / 2.f);
		}
		sprite.setScale(data.scale);
	}
}

sf::Sprite* SpriteMgr::getSprite(std::string_view name) {
	if(name.empty()) {Logger::error_throw("Sprite name is empty");}
	if (auto it = inst().sprites.find(name); it != inst().sprites.end()) {
		return &it->second;
	}
	Logger::error_throw("No such sprite: {}", name);
}
