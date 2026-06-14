#include "../global/SpriteManager.h"
#include "AssetManager.h"
#include "DataManager.h"
#include <SFML/Graphics.hpp>

namespace flx::app {
	class SpriteMgrImpl {
		FLX_DECL_SINGLETON(SpriteMgrImpl);
		void initSprites();

	public:
		flx::StrMap<sf::Sprite> sprites;

		SpriteMgrImpl() {
			initSprites();
		}
	};

	static SpriteMgrImpl& inst() {
		return SpriteMgrImpl::inst();
	}

	void SpriteMgrImpl::initSprites() {
		for (auto& [name, data] : DataMgr::getSpriteData()) {
			auto& texture = AssetMgr::getSpriteTexture(data.texture);
			auto [it, state] = sprites.emplace(std::piecewise_construct,
											   std::forward_as_tuple(name),
											   std::forward_as_tuple(texture));
			if (!state) {
				flx::logger.warn("Sprite name {} is dulplicated! Skipped loading.", name);
				continue;
			}
			auto& sprite = it->second;
			if (data.centerAligned) {
				sprite.setOrigin(sprite.getLocalBounds().size / 2.f);
			}
			if (data.targetSize != vec2{}) {
				sprite.setScale(data.targetSize / vec2(texture.getSize()));
			} else {
				sprite.setScale(data.scale);
			}
		}
	}

	const sf::Sprite& SpriteMgr::getSprite(std::string_view name) {
		if (const auto sprite = inst().sprites.try_find(name)) {
			return *sprite;
		}
		flx::logger.error_and_throw("Cannot find sprite: {}", name);
	}
} // namespace flx::game