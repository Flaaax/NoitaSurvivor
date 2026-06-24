#pragma once
#include "context/NInputState.h"
#include "context/WindowEvent.h"
#include "render/NWindowView.h"

namespace flx::ui {
	class RenderBuffer;

	// Manages and holds the SFML window
	class Window {
	public:
		enum Mode {
			Windowed,
			Borderless,
			Fullscreen,
		};

	private:
		sf::RenderWindow window;
		NWindowView viewport;
		Mode mode = Mode::Windowed;
		std::string title{};

		void updateViewport();
		void updateMousePosition();

	public:
		NInputState input;

		// Creates the window
		Window(vec2u windowSize, std::string_view title);
		~Window();

		const NWindowView& getView() const;

		sf::RenderWindow& getWindow() {
			return window;
		}

		bool isOpen() const {
			return window.isOpen();
		}

		std::optional<WindowEvent> pollEvent();

		void draw(RenderBuffer& buffer);
		void display();

		Mode getMode() const;
		void setMode(Mode mode);
	};
} // namespace flx::ui