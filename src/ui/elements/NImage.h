#pragma once
#include "src/ui/NObject.h"

#include <SFML/Graphics/Sprite.hpp>

namespace flx::ui {
	class NImage : public NObject {
	protected:
		mutable ::sf::Sprite sprite;
		float outLineWidth{};
		::sf::Color outlineColor{};

	public:
		explicit NImage(const ::sf::Texture& texture);

		void draw(const NUIPainter& canvas) const override;
		void setOutlineWidth(float width);
		void setOutlineColor(::sf::Color color);
	};
} // namespace flx::ui