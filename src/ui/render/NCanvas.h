#pragma once
#include "../../utils/Vec2/Vec2.h"
#include "src/render/Renderer.h"

#include <SFML/Graphics/RenderStates.hpp>

class NCanvas {
private:
	Renderer& rdr;
	sf::RenderStates states;

public:
	NCanvas translated(nvec2 offset) const {
		return NCanvas(rdr, offset, states);
	}

	explicit NCanvas(Renderer& rdr, nvec2 translate, const sf::RenderStates& states = sf::RenderStates::Default) : rdr(rdr), states(states) {
		this->states.transform.translate(translate);
	}

	void draw(const sf::Drawable& drawable) const {
		rdr.drawUI(drawable, states);
	}
};