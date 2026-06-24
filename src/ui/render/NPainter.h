#pragma once
#include "../../utils/Vec2/Vec2.h"
#include "NRenderBuffer.h"
#include <SFML/Graphics/RenderStates.hpp>

namespace flx::ui {
	class Painter {
	private:
		RenderBuffer& rdr;

	public:
		sf::RenderStates states;

		Painter translated(vec2 offset) const {
			return Painter(rdr, offset, states);
		}

		explicit Painter(RenderBuffer& rdr, vec2 translate, const sf::RenderStates& states = sf::RenderStates::Default) : Painter(rdr, states) {
			this->states.transform.translate(translate);
		}

		explicit Painter(RenderBuffer& rdr, const sf::RenderStates& states = sf::RenderStates::Default) : rdr(rdr), states(states) {
		}

		void draw(const ::sf::Drawable& drawable) const {
			rdr.drawCanvas(drawable, states);
		}
	};

	class NUIPainter {
	private:
		RenderBuffer& rdr;

	public:
		::sf::RenderStates states;

		NUIPainter translated(vec2 offset) const {
			return NUIPainter(rdr, offset, states);
		}

		explicit NUIPainter(RenderBuffer& rdr, vec2 translate, const sf::RenderStates& states = sf::RenderStates::Default) : NUIPainter(rdr, states) {
			this->states.transform.translate(translate);
		}

		explicit NUIPainter(RenderBuffer& rdr, const sf::RenderStates& states = sf::RenderStates::Default) : rdr(rdr), states(states) {
		}

		void draw(const sf::Drawable& drawable) const {
			rdr.drawUI(drawable, states);
		}

		void drawText(const NRichTextShape& text) const {
			rdr.drawUIText(text, states);
		}
	};
} // namespace flx::ui