#pragma once
#include "../../utils/Vec2/Vec2.h"
#include <SFML/Graphics.hpp>

namespace flx::ui {
	class LineShape : public sf::Drawable {
	public:
		explicit LineShape() {
			vertices.setPrimitiveType(sf::PrimitiveType::Lines);
			vertices.resize(2ull);
		}

		LineShape(vec2 start, vec2 end, sf::Color color) {
			vertices.setPrimitiveType(sf::PrimitiveType::Lines);
			vertices.append({start, color});
			vertices.append({end, color});
			/*vertices.resize(2);
			vertices[0].position = start;
			vertices[0].color = color;
			vertices[1].position = end;
			vertices[1].color = color;*/
		}

		void set(vec2 start, vec2 end, sf::Color color) {
			// vertices.setPrimitiveType(sf::Lines);
			vertices[0] = {start, color};
			vertices[1] = {end, color};
		}

		sf::VertexArray vertices;

	protected:
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
			target.draw(vertices, states);
		}
	};
} // namespace flx::ui