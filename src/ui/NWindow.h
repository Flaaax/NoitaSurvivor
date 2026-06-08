#pragma once
#include "context/NInputState.h"
#include "context/NWindowEvent.h"
#include "render/NWindowViewport.h"

class NRenderBuffer;

// Manages and creates the SFML window
class NWindow {
private:
	sf::RenderWindow window;
	NWindowViewport viewport;

	void updateViewport();
	void updateMousePosition();

public:
	NInputState input;

	NWindow(nvec2u windowSize, std::string_view title);
	~NWindow();

	const NWindowViewport& getViewport()const;

	sf::RenderWindow& getWindow() {
		return window;
	}

	bool isOpen() const {
		return window.isOpen();
	}

	std::optional<NWindowEvent> pollEvent();

	void draw(NRenderBuffer& buffer);
	void display();
};