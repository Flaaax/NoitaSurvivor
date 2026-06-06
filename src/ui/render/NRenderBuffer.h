#pragma once
#include "NWindowViewport.h"

#include <SFML/Graphics/RenderTexture.hpp>

namespace sf {
	class RenderWindow;
}

class NRenderBuffer {
private:
	friend class NWindow;

	sf::RenderTexture texture;
	sf::RenderTexture accumTexture;

	void flush(sf::RenderWindow& window, const NWindowViewport& viewport);

public:
	sf::Color clearColor{};
	bool enableMotionBlur = true;

	explicit NRenderBuffer(const NWindowViewport& viewport);

	void draw(const sf::Drawable& content, const sf::RenderStates& states = sf::RenderStates::Default);
	void clear(sf::Color color);
};