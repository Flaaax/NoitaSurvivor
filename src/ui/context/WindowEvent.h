#pragma once

#include <SFML/Window/Event.hpp>

namespace flx::ui {
	using RawEvent = sf::Event;
	struct WindowView;
	struct InputState;

	struct WindowEvent {
		RawEvent rawEvent;
		const WindowView& viewport;
		const InputState& input;
	};

} // namespace flx::ui
