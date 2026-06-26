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
			const vec2 size = getSize();

			if (keepAspectRatio) {
				const float scaleValue = std::min(size.x / textureSize.x, size.y / textureSize.y);
				sprite.setScale({scaleValue, scaleValue});

				const vec2 scaledSize = textureSize * scaleValue;
				sprite.setPosition((size - scaledSize) / 2.f);
			} else {
				sprite.setScale(size / textureSize);
				sprite.setPosition({0.f, 0.f});
			}

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

	void Image::setKeepAspectRatio(bool keep) {
		if (keep == keepAspectRatio) {
			return;
		}
		keepAspectRatio = keep;
		visualDirty = true;
	}
} // namespace flx::ui