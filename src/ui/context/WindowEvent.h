#pragma once

#include <SFML/Window/Event.hpp>

namespace flx::ui {
	using NRawEvent = sf::Event;
	struct WindowView;
	struct InputState;

	struct WindowEvent {
		NRawEvent rawEvent;
		const WindowView& viewport;
		const InputState& input;
	};

} // namespace flx::ui
