#include "NImage.h"

#include "src/ui/render/NCanvas.h"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

NImage::NImage(const sf::Texture& texture) : sprite(texture) {
	visualDirty = true;
}

void NImage::draw(const NCanvas& canvas) const {
	if (visualDirty) {
		const nvec2 textureSize = static_cast<nvec2>(sprite.getTexture().getSize());
		const nvec2 scale = getSize() / textureSize;
		sprite.setScale(scale);
		visualDirty = false;
	}
	canvas.draw(sprite);
}