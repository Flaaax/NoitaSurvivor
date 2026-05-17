#pragma once
#include "src/render/Renderer.h"
#include "src/utils/Vec2.h"

#include <SFML/Graphics/RenderStates.hpp>

class NCanvas {
private:
	Renderer& rdr;
	sf::RenderStates states;

public:
	NCanvas translated(const nvec2& offset) const {
		NCanvas child{rdr, states};
		child.states.transform.translate(offset);
		return child;
	}

	explicit NCanvas(Renderer& rdr, const sf::RenderStates& states = sf::RenderStates::Default) : rdr(rdr), states(states) {
	}

	void draw(const sf::Drawable& drawable) const {
		rdr.drawUI(drawable, states);
	}
};