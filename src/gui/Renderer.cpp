#include "Renderer.h"

void Renderer::draw(sf::RenderWindow& window) {
	texture.display();
	sf::Sprite sprite;
	if (enableMotionBlur) {
		sf::Sprite tempSprite;
		float accum_k = 0.95f;
		float new_k = 0.05f;
		//if there's no accum texture
		if (accumTexture.getSize().x == 0u) {
			accum_k = 0.f;
			new_k = 1.f;
		}

		float accum_alpha = accum_k;
		float new_alpha = new_k / (1 - accum_alpha); // alpha correction

		tempSprite.setColor({ 255,255,255,static_cast<sf::Uint8>(std::roundf(255 * accum_alpha)) });
		tempSprite.setTexture(accumTexture.getTexture());
		accumTexture.clear();
		accumTexture.draw(tempSprite);
		tempSprite.setColor({ 255,255,255,static_cast<sf::Uint8>(std::roundf(255 * new_alpha)) });
		tempSprite.setTexture(texture.getTexture());
		accumTexture.draw(tempSprite);
		accumTexture.display();
		sprite.setTexture(accumTexture.getTexture());
	}
	else {
		sprite.setTexture(texture.getTexture());
	}

	sprite.setScale(NWindow::scale.currentRenderScale);		//because the coords of sf::RenderTexture are flipped, y=0 is below
	sprite.setPosition(NWindow::scale.realRenderOffset.x, NWindow::scale.realRenderOffset.y);
	window.clear(sf::Color::Black);
	window.draw(sprite);
}
