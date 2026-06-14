#pragma once
#include "context/NInputState.h"
#include "context/NWindowEvent.h"
#include "render/NWindowView.h"

namespace flx::ui {
	class NRenderBuffer;

	// Manages and holds the SFML window
	class NWindow {
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
		NWindow(vec2u windowSize, std::string_view title);
		~NWindow();

		const NWindowView& getView() const;

		sf::RenderWindow& getWindow() {
			return window;
		}

		bool isOpen() const {
			return window.isOpen();
		}

		std::optional<NWindowEvent> pollEvent();

		void draw(NRenderBuffer& buffer);
		void display();

		Mode getMode() const;
		void setMode(Mode mode);
	};
} // namespace flx::ui