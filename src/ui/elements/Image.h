#pragma once
#include "src/ui/Object.h"

#include <SFML/Graphics/Sprite.hpp>

namespace flx::ui {
	class Image : public Object {
	protected:
		mutable ::sf::Sprite sprite;
		float outLineWidth{};
		::sf::Color outlineColor{};

	public:
		explicit Image(const ::sf::Texture& texture);

		void draw(const UIPainter& canvas) const override;
		void setOutlineWidth(float width);
		void setOutlineColor(::sf::Color color);
	};
} // namespace flx::ui