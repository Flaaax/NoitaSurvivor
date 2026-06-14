#include "NImage.h"

#include "src/ui/render/NPainter.h"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace flx::ui {
	NImage::NImage(const ::sf::Texture& texture) : sprite(texture) {
		visualDirty = true;
	}

	void NImage::draw(const NUIPainter& canvas) const {
		if (visualDirty) {
			const vec2 textureSize = static_cast<vec2>(sprite.getTexture().getSize());
			const vec2 scale = getSize() / textureSize;
			sprite.setScale(scale);
			visualDirty = false;
		}

		if (outLineWidth > 0) {
			sf::RectangleShape shape;
			shape.setFillColor({0, 0, 0, 0});
			shape.setOutlineColor(outlineColor);
			shape.setOutlineThickness(outLineWidth);
			shape.setSize(getSize());
			canvas.draw(shape);
		}

		canvas.draw(sprite);
	}

	void NImage::setOutlineWidth(float width) {
		outLineWidth = width;
		visualDirty = true;
	}

	void NImage::setOutlineColor(::sf::Color color) {
		outlineColor = color;
		visualDirty = true;
	}
} // namespace flx::ui