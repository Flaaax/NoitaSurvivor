#pragma once
#include<SFML/Graphics.hpp>
#include"src/utils/Vec2.h"


class NLineShape : public sf::Drawable {
public:
	explicit NLineShape() {
		vertices.setPrimitiveType(sf::Lines);
		vertices.resize(2ull);
	}

	NLineShape(const nvec2& start, const nvec2& end, const sf::Color& color) {
		vertices.setPrimitiveType(sf::Lines);
		vertices.append({ start.as_fvec2, color });
		vertices.append({ end.as_fvec2, color });
		/*vertices.resize(2);
		vertices[0].position = start;
		vertices[0].color = color;
		vertices[1].position = end;
		vertices[1].color = color;*/
	}

	void set(const nvec2& start, const nvec2& end, const sf::Color& color) {
		//vertices.setPrimitiveType(sf::Lines);
		vertices[0] = { start.as_fvec2, color };
		vertices[1] = { end.as_fvec2, color };
	}

	sf::VertexArray vertices;

private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		target.draw(vertices, states);
	}
};

