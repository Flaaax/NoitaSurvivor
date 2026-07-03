#include "Window.h"

#include "../utils/Logging/Logger.h"
#include "global/Global.h"
#include "render/RenderBuffer.h"
#include <SFML/Graphics.hpp>

namespace flx::ui {
	void Window::updateViewport() {
		viewport.resize(window.getSize());
		const auto size = static_cast<vec2>(window.getSize());
		// glViewport(0, 0, sizeu.x, sizeu.y);

		sf::View view;
		view.setCenter(size / 2.f);
		view.setSize(size);
		window.setView(view);
	}

	void Window::updateMousePosition() {
		input.mouseScreen = static_cast<vec2>(sf::Mouse::getPosition(window));
		input.mouseRender = viewport.toCanvasPos(input.mouseScreen);
	}

	Window::Window(vec2u windowSize, std::string_view title)
		: window(sf::VideoMode(windowSize), title.data()), viewport(windowSize) {

		updateViewport();
		updateMousePosition();

		this->title = title;

		Global::getLogger().info("Window opened: {}", title);
	}

	Window::~Window() {
	}

	const WindowView& Window::getView() const {
		return viewport;
	}

	const InputState& Window::getInput() const {
		return input;
	}

	std::optional<WindowEvent> Window::pollEvent() {
		if (const auto event = window.pollEvent()) {
			if (event->is<sf::Event::Resized>()) {
				updateViewport();
			}

			if (event->is<sf::Event::MouseMoved>() ||
				event->is<sf::Event::FocusGained>() ||
				event->is<sf::Event::FocusLost>()) {
				updateMousePosition();
			}

			return WindowEvent{
				.rawEvent = *event,
				.viewport = getView(),
				.input = input,
			};
		}

		return {};
	}

	void Window::draw(RenderBuffer& buffer) {
		buffer.flush(window);
	}

	void Window::display() {
		window.display();
	}

	WindowMode Window::getMode() const {
		return mode;
	}

	void Window::setMode(WindowMode mode) {
		if (mode == this->mode) {
			Global::getLogger().warn("NWindow: Did not change window mode");
			return;
		}

		this->mode = mode;

		if (mode == WindowMode::Windowed) {
			window.create(sf::VideoMode(viewport.defaultWindowSize), title);
		} else if (mode == WindowMode::Borderless) {
			const auto desktop = sf::VideoMode::getDesktopMode();
			window.create(desktop, title, sf::Style::None);
		} else if (mode == WindowMode::Fullscreen) {
			const auto desktop = sf::VideoMode::getDesktopMode();
			window.create(desktop, title, sf::Style::None, sf::State::Fullscreen);
		}

		updateViewport();
		updateMousePosition();
	}
} // namespace flx::ui