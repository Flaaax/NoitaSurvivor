#include"Spell.h"
#include"src/global/AssetManager.h"



sf::Texture& Spell::getTexture() {
	if (!texture) {
		texture = AssetMgr::getSpellTexture(textureName);
	}
	return *texture;
}