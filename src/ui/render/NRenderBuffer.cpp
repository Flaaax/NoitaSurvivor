#include "NRenderBuffer.h"

#include "src/ui/global/NGlobal.h"
#include "src/ui/shapes/NRichTextShape.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>

namespace flx::ui {
	NRenderBuffer::NRenderBuffer(const NWindowView& viewport)
		: canvasTexture(viewport.defaultWindowSize),
		  canvasAccumTexture(viewport.defaultWindowSize),
		  UITexture(viewport.defaultWindowSize) {

		onWindowResized(viewport);
	}

	void NRenderBuffer::onWindowResized(const NWindowView& view) {
		if (!UITexture.resize(view.windowSize)) {
			NGlobal::getLogger().error_and_throw("Failed to resize render texture with unknown reason");
		}

		viewport = view.viewport;

		canvasTransform = viewport.getTransformToScreen();

		canvasTexture.clear(clearColor);
		UITexture.clear(clearColor);
	}

	void NRenderBuffer::drawCanvas(const sf::Drawable& content, const sf::RenderStates& states) {
		canvasTexture.draw(content, states);
	}

	void NRenderBuffer::drawUI(const sf::Drawable& content, sf::RenderStates states) {
		states.transform = canvasTransform * states.transform;
		UITexture.draw(content, states);
	}

	void NRenderBuffer::drawUIText(const NRichTextShape& text, sf::RenderStates states) {
		const vec2 canvasPos = states.transform.transformPoint({0.f, 0.f});
		states.transform = sf::Transform().translate(viewport.offset + canvasPos * viewport.scale);
		UITexture.draw(text, states);
	}

	void NRenderBuffer::drawUIRaw(const sf::Drawable& content, const sf::RenderStates& states) {
		UITexture.draw(content, states);
	}

	void NRenderBuffer::clear(sf::Color color) {
		canvasTexture.clear(color);
		UITexture.clear(sf::Color::Transparent);
	}

	NViewport NRenderBuffer::getViewport() const {
		return viewport;
	}

	void NRenderBuffer::flush(sf::RenderWindow& window) {
		canvasTexture.display();
		const sf::Texture* targetTexture{};

		if (enableMotionBlur) {
			float accum_k = 0.95f;
			float new_k = 0.05f;
			// If it's the first frame
			if (canvasAccumTexture.getSize().x == 0u) {
				accum_k = 0.f;
				new_k = 1.f;
			}

			const float accum_alpha = accum_k;
			const float new_alpha = new_k / (1 - accum_alpha); // alpha correction

			sf::Sprite tempSprite(canvasAccumTexture.getTexture());
			tempSprite.setColor({255, 255, 255, static_cast<std::uint8_t>(std::roundf(255 * accum_alpha))});
			canvasAccumTexture.clear();
			canvasAccumTexture.draw(tempSprite);
			tempSprite.setColor({255, 255, 255, static_cast<std::uint8_t>(std::roundf(255 * new_alpha))});
			tempSprite.setTexture(canvasTexture.getTexture());
			canvasAccumTexture.draw(tempSprite);
			canvasAccumTexture.display();

			targetTexture = &canvasAccumTexture.getTexture();
		} else {
			targetTexture = &canvasTexture.getTexture();
		}

		sf::Sprite sprite(*targetTexture);

		sprite.setScale(viewport.scale);
		sprite.setPosition(viewport.offset);
		window.clear(clearColor);
		window.draw(sprite);

		UITexture.display();
		sf::Sprite UISprite(UITexture.getTexture());
		window.draw(UISprite);
	}
} // namespace flx::ui