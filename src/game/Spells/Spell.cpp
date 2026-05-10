#include"Spell.h"
#include"src/global/AssetManager.h"


sf::Texture& Spell::getTexture() {
	if (!texture) {
		if (ID.empty()) {
			Logger::warn("Spell '{}' does not have a texture", typeid(*this).name());
		}
		texture = AssetMgr::getSpellTexture(ID);
	}
	return *texture;
}