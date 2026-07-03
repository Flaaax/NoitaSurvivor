#pragma once
#include "context/InputState.h"
#include "context/WindowEvent.h"
#include "context/WindowMode.h"
#include "render/WindowView.h"

namespace flx::ui {
	class RenderBuffer;

	// Manages and holds the SFML window
	class Window {
	private:
		using enum WindowMode;

		sf::RenderWindow window;
		WindowView viewport;
		WindowMode mode = Windowed;
		InputState input;
		std::string title{};

		void updateViewport();
		void updateMousePosition();

	public:

		// Creates the window
		Window(vec2u windowSize, std::string_view title);
		~Window();

		const WindowView& getView() const;
		const InputState& getInput()const;

		sf::RenderWindow& getWindow() {
			return window;
		}

		bool isOpen() const {
			return window.isOpen();
		}

		std::optional<WindowEvent> pollEvent();

		void draw(RenderBuffer& buffer);
		void display();

		WindowMode getMode() const;
		void setMode(WindowMode mode);
	};
} // namespace flx::ui