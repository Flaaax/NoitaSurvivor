#pragma once
#include "../../utils/Vec2/Vec2.h"
#include "NRenderBuffer.h"
#include <SFML/Graphics/RenderStates.hpp>

class NPainter {
private:
	NRenderBuffer& rdr;

public:
	sf::RenderStates states;

	NPainter translated(nvec2 offset) const {
		return NPainter(rdr, offset, states);
	}

	explicit NPainter(NRenderBuffer& rdr, nvec2 translate, const sf::RenderStates& states = sf::RenderStates::Default) : NPainter(rdr, states) {
		this->states.transform.translate(translate);
	}

	explicit NPainter(NRenderBuffer& rdr, const sf::RenderStates& states = sf::RenderStates::Default) : rdr(rdr), states(states) {
	}

	void draw(const sf::Drawable& drawable) const {
		rdr.draw(drawable, states);
	}
};