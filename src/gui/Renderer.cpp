#include "Renderer.h"

void Renderer::draw(sf::RenderWindow& window) {
	texture.display();
	const sf::Texture* targetTexture{};

	if (enableMotionBlur) {
		float accum_k = 0.95f;
		float new_k = 0.05f;
		// If it's the first frame
		if (accumTexture.getSize().x == 0u) {
			accum_k = 0.f;
			new_k = 1.f;
		}

		const float accum_alpha = accum_k;
		const float new_alpha = new_k / (1 - accum_alpha); // alpha correction

		sf::Sprite tempSprite(accumTexture.getTexture());
		tempSprite.setColor({255, 255, 255, static_cast<std::uint8_t>(std::roundf(255 * accum_alpha))});
		accumTexture.clear();
		accumTexture.draw(tempSprite);
		tempSprite.setColor({255, 255, 255, static_cast<std::uint8_t>(std::roundf(255 * new_alpha))});
		tempSprite.setTexture(texture.getTexture());
		accumTexture.draw(tempSprite);
		accumTexture.display();

		targetTexture = &accumTexture.getTexture();
	} else {
		targetTexture = &texture.getTexture();
	}

	sf::Sprite sprite(*targetTexture);

	sprite.setScale(NWindow::scale.currentRenderScale); // because the coords of sf::RenderTexture are flipped, y=0 is below
	sprite.setPosition(NWindow::scale.realRenderOffset);
	window.clear(sf::Color::Black);
	window.draw(sprite);
}
