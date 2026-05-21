#pragma once
#include "src/ui/NWindow.h"
#include "src/ui/NScale.h"
#include <SFML/Graphics.hpp>

class Renderer {
	friend class NWidget;

private:
	sf::RenderTexture texture;
	sf::RenderTexture accumTexture;
	sf::RenderStates gameContentState;
	bool enableMotionBlur = true;

public:
	Renderer() : texture(NScale::defaultWindowSize),
				 accumTexture(NScale::defaultWindowSize) {
	}

	// should be called in class Game
	void updateGameRender(nvec2 cameraRenderPos) {
		gameContentState.transform = sf::Transform::Identity;
		gameContentState.transform
			.translate(-cameraRenderPos)
			.scale(NScale::gameRenderScale);
	}

	void drawGame(const sf::Drawable& gameContent) {
		texture.draw(gameContent, gameContentState);
	}

	void drawUI(const sf::Drawable& guiContent, const sf::RenderStates states) {
		texture.draw(guiContent, states);
	}

	void clear(sf::Color color = {0, 0, 0}) {
		texture.clear(color);
	}

	/*void clearEmptyArea() {
		auto rect = NScale::renderArea;
		auto currentWindowSize = NScale::currentWindowSize;

		float left = rect.x;
		float right = rect.x + rect.w;
		float top = rect.y;
		float bottom = rect.y + rect.h;

		sf::RectangleShape blackRect;
		blackRect.setSize({ left, currentWindowSize.y });
		blackRect.setPosition(0, 0);
		blackRect.setFillColor(sf::Color::Black);
		window->draw(blackRect);
		blackRect.setSize({ currentWindowSize.x - right, currentWindowSize.y });
		blackRect.setPosition(right, 0);
		window->draw(blackRect);
		blackRect.setSize({ rect.w, top });
		blackRect.setPosition(left, 0);
		window->draw(blackRect);
		blackRect.setSize({ rect.w, currentWindowSize.y - bottom });
		blackRect.setPosition(left, bottom);
		window->draw(blackRect);
	}*/

	static void clone(const sf::RenderTexture& from, sf::RenderTexture& to) {
		const sf::Sprite s(from.getTexture());
		to.draw(s);
	}

	// Ensures that y=0 is at the bottom of the screen
	void draw(sf::RenderWindow& window);
};
