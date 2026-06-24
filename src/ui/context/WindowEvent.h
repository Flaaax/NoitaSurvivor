#pragma once

#include <SFML/Window/Event.hpp>

namespace flx::ui {
	using NRawEvent = sf::Event;
	struct NWindowView;
	struct NInputState;

	struct WindowEvent {
		NRawEvent rawEvent;
		const NWindowView& viewport;
		const NInputState& input;
	};

} // namespace flx::ui
