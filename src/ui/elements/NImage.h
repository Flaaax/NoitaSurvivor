#pragma once
#include "src/ui/Object.h"

#include <SFML/Graphics/Sprite.hpp>

namespace flx::ui {
	class NImage : public Object {
	protected:
		mutable ::sf::Sprite sprite;
		float outLineWidth{};
		::sf::Color outlineColor{};

	public:
		explicit NImage(const ::sf::Texture& texture);

		void draw(const UIPainter& canvas) const override;
		void setOutlineWidth(float width);
		void setOutlineColor(::sf::Color color);
	};
} // namespace flx::ui