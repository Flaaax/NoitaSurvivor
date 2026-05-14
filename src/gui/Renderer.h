#pragma once
#include<SFML/Graphics.hpp>
#include"src/utils/Math.h"
#include"NWindow.h"


class Renderer {
	friend class NWidget;
private:
	sf::RenderTexture texture;
	sf::RenderTexture accumTexture;
	sf::RenderStates gameContentState;
	sf::RenderStates guiContentState;
	bool enableMotionBlur = false;

public:
	Renderer() {
		texture.create(NScale::defaultWindowSizeX, NScale::defaultWindowSizeY);
		accumTexture.create(NScale::defaultWindowSizeX, NScale::defaultWindowSizeY);
	}

	//should be called in class Game
	void updateGameRender(const fvec2& cameraRenderPos) {
		gameContentState.transform = sf::Transform::Identity;
		gameContentState.transform
			.translate(-cameraRenderPos)
			.scale(NScale::gameRenderScale);
	}

	void updateGuiRender() {
		guiContentState.transform = sf::Transform::Identity;
		//guiContentState.transform.scale(1.f, -1.f);
		//guiContentState.transform.translate(NScale::realRenderOffset);
		//guiContentState.transform.scale(NScale::currentRenderScale.Fvec2());
	}

	void drawGame(const sf::Drawable& gameContent) {
		texture.draw(gameContent, gameContentState);
	}

	void drawGui(const sf::Drawable& guiContent) {
		texture.draw(guiContent, guiContentState);
	}

	void clear(const sf::Color color = { 0,0,0 }) {
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
		sf::Sprite s;
		s.setTexture(from.getTexture());
		to.draw(s);
	}

	//Ensures that y=0 is at below
	void draw(sf::RenderWindow& window);
};
