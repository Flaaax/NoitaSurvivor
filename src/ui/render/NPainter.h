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
		rdr.drawCanvas(drawable, states);
	}
};

class NUIPainter {
private:
	NRenderBuffer& rdr;

public:
	sf::RenderStates states;

	NUIPainter translated(nvec2 offset) const {
		return NUIPainter(rdr, offset, states);
	}

	explicit NUIPainter(NRenderBuffer& rdr, nvec2 translate, const sf::RenderStates& states = sf::RenderStates::Default) : NUIPainter(rdr, states) {
		this->states.transform.translate(translate);
	}

	explicit NUIPainter(NRenderBuffer& rdr, const sf::RenderStates& states = sf::RenderStates::Default) : rdr(rdr), states(states) {
	}

	void draw(const sf::Drawable& drawable) const {
		rdr.drawUI(drawable, states);
	}

	void drawText(const NRichTextShape& text) const {
		rdr.drawUIText(text, states);
	}
};