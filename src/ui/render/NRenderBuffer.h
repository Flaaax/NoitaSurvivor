#pragma once
#include "NViewport.h"
#include "NWindowView.h"
#include "src/utils/Integers.h"

#include <SFML/Graphics/RenderTexture.hpp>

namespace sf {
	class RenderWindow;
}

namespace flx::ui {
	class NRichTextShape;

	class RenderBuffer {
	private:
		friend class Window;

		NViewport viewport;
		sf::Transform canvasTransform;

		sf::RenderTexture canvasTexture;
		sf::RenderTexture canvasAccumTexture;
		sf::RenderTexture UITexture;

		void flush(sf::RenderWindow& window);

	public:
		sf::Color clearColor{};
		bool enableMotionBlur = true;

		explicit RenderBuffer(const NWindowView& viewport);

		void onWindowResized(const NWindowView& view);

		void drawCanvas(const sf::Drawable& content, const sf::RenderStates& states = sf::RenderStates::Default);
		// Special entrance for UI elements
		void drawUI(const sf::Drawable& content, sf::RenderStates states = sf::RenderStates::Default);
		// This doesn't scale the text, so you should set character px manually
		void drawUIText(const NRichTextShape& text, sf::RenderStates states = sf::RenderStates::Default);
		void drawUIRaw(const sf::Drawable& content, const sf::RenderStates& states = sf::RenderStates::Default);

		void clear(sf::Color color);

		NViewport getViewport() const;
	};
} // namespace flx::ui