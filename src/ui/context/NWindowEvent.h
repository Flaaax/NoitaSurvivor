#pragma once

#include <SFML/Window/Event.hpp>

namespace flx::ui {
	using NRawEvent = sf::Event;
	struct NWindowView;
	struct NInputState;

	struct NWindowEvent {
		NRawEvent rawEvent;
		const NWindowView& viewport;
		const NInputState& input;
	};

} // namespace flx::ui
