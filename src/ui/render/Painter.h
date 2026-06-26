#pragma once
#include "../../utils/Vec2/Vec2.h"
#include "RenderBuffer.h"
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

		explicit Painter(RenderBuffer& rdr, vec2 translate, const sf::RenderStates& states = sf::RenderStates::Default)
			: Painter(rdr, states) {
			this->states.transform.translate(translate);
		}

		explicit Painter(RenderBuffer& rdr, const sf::RenderStates& states = sf::RenderStates::Default)
			: rdr(rdr), states(states) {
		}

		void draw(const ::sf::Drawable& drawable) const {
			rdr.drawCanvas(drawable, states);
		}
	};

	class UIPainter {
	private:
		RenderBuffer& rdr;

	public:
		sf::RenderStates states;

		UIPainter translated(vec2 offset) const {
			return UIPainter(rdr, offset, states);
		}

		explicit UIPainter(RenderBuffer& rdr, vec2 translate, const sf::RenderStates& states = sf::RenderStates::Default)
			: UIPainter(rdr, states) {
			this->states.transform.translate(translate);
		}

		explicit UIPainter(RenderBuffer& rdr, const sf::RenderStates& states = sf::RenderStates::Default)
			: rdr(rdr), states(states) {
		}

		void draw(const sf::Drawable& drawable) const {
			rdr.drawUI(drawable, states);
		}

		void drawText(const RichTextShape& text) const {
			rdr.drawUIText(text, states);
		}
	};
} // namespace flx::ui