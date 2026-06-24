#include "Image.h"

#include "src/ui/render/Painter.h"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace flx::ui {
	Image::Image(const ::sf::Texture& texture) : sprite(texture) {
		visualDirty = true;
	}

	void Image::draw(const UIPainter& canvas) const {
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

	void Image::setOutlineWidth(float width) {
		outLineWidth = width;
		visualDirty = true;
	}

	void Image::setOutlineColor(::sf::Color color) {
		outlineColor = color;
		visualDirty = true;
	}
} // namespace flx::ui