#include "NWindow.h"

#include "../utils/Logging/Logger.h"
#include "global/NGlobal.h"
#include "render/NRenderBuffer.h"
#include <SFML/Graphics.hpp>

void NWindow::updateViewport() {
	viewport.resize(window.getSize());
	const auto size = static_cast<nvec2>(window.getSize());
	// glViewport(0, 0, sizeu.x, sizeu.y);

	sf::View view;
	view.setCenter(size / 2.f);
	view.setSize(size);
	window.setView(view);
}

void NWindow::updateMousePosition() {
	input.mouseScreen = static_cast<nvec2>(sf::Mouse::getPosition(window));
	input.mouseRender = viewport.toCanvasPos(input.mouseScreen);
}

NWindow::NWindow(nvec2u windowSize, std::string_view title)
	: window(sf::VideoMode(windowSize), title.data()), viewport(windowSize) {

	updateViewport();
	updateMousePosition();

	this->title = title;

	NGlobal::getLogger().info("Window opened: {}", title);
}

NWindow::~NWindow() {
}

const NWindowView& NWindow::getView() const {
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
			.viewport = getView(),
			.input = input,
		};
	}

	return {};
}

void NWindow::draw(NRenderBuffer& buffer) {
	buffer.flush(window);
}

void NWindow::display() {
	window.display();
}

NWindow::Mode NWindow::getMode() const {
	return mode;
}

void NWindow::setMode(Mode mode) {
	if (mode == this->mode) {
		NGlobal::getLogger().warn("NWindow: Did not change window mode");
		return;
	}

	this->mode = mode;

	if (mode == Mode::Windowed) {
		window.create(sf::VideoMode(viewport.defaultWindowSize), title);
	} else if (mode == Mode::Borderless) {
		const auto desktop = sf::VideoMode::getDesktopMode();
		window.create(desktop, title, sf::Style::None);
	} else if (mode == Mode::Fullscreen) {
		const auto desktop = sf::VideoMode::getDesktopMode();
		window.create(desktop, title, sf::Style::None, sf::State::Fullscreen);
	}

	updateViewport();
	updateMousePosition();
}