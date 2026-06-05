#pragma warning(disable : 5105)
#include "NWindow.h"
#include "render/NRenderBuffer.h"
#include "src/utils/Logger.h"
#include <SFML/Graphics.hpp>

void NWindow::updateViewport() {
	const auto size = static_cast<nvec2>(window.getSize());
	viewport.update(size);
	// glViewport(0, 0, sizeu.x, sizeu.y);

	sf::View view;
	view.setCenter(size / 2.f);
	view.setSize(size);
	window.setView(view);
}

void NWindow::updateMousePosition() {
	input.mouseScreen = static_cast<nvec2>(sf::Mouse::getPosition(window));
	input.mouseRender = viewport.toRenderPos(input.mouseScreen);
}

NWindow::NWindow(nvec2u windowSize, std::string_view title)
	: window(sf::VideoMode(windowSize), title.data()), viewport(windowSize) {

	updateViewport();
	updateMousePosition();

	flx::logger.info("Window opened: {}", title);
}

NWindow::~NWindow() {
}

const NWindowViewport& NWindow::getViewport() const {
	return viewport;
}

std::optional<NWindowEvent> NWindow::pollEvent() {
	if (const auto event = window.pollEvent()) {
		if (event->is<sf::Event::Resized>()) {
			updateViewport();
		}

		if (event->is<sf::Event::MouseMoved>() ||
			event->is<sf::Event::FocusGained>() ||
			event->is<sf::Event::FocusLost>()) {
			updateMousePosition();
		}

		return NWindowEvent{
			.rawEvent = *event,
			.viewport = getViewport(),
			.input = input,
		};
	}

	return {};
}

void NWindow::draw(NRenderBuffer& buffer) {
	buffer.flush(window, getViewport());
	window.display();
}
