#pragma once
#include "Viewport.h"
#include "WindowView.h"
#include "src/utils/Integers.h"

#include <SFML/Graphics/RenderTexture.hpp>

namespace sf {
	class RenderWindow;
}

namespace flx::ui {
	class RichTextShape;

	class RenderBuffer {
	private:
		friend class Window;

		Viewport viewport;
		sf::Transform canvasTransform;

		sf::RenderTexture canvasTexture;
		sf::RenderTexture canvasAccumTexture;
		sf::RenderTexture UITexture;

		void flush(sf::RenderWindow& window);

	public:
		sf::Color clearColor{};
		bool enableMotionBlur = true;

		explicit RenderBuffer(const WindowView& viewport);

		void onWindowResized(const WindowView& view);

		void drawCanvas(const sf::Drawable& content, const sf::RenderStates& states = sf::RenderStates::Default);
		// Special entrance for UI elements
		void drawUI(const sf::Drawable& content, sf::RenderStates states = sf::RenderStates::Default);
		// This doesn't scale the text, so you should set character px manually
		void drawUIText(const RichTextShape& text, sf::RenderStates states = sf::RenderStates::Default);
		void drawUIRaw(const sf::Drawable& content, const sf::RenderStates& states = sf::RenderStates::Default);

		void clear(sf::Color color);

		Viewport getViewport() const;
	};
} // namespace flx::ui