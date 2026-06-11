#pragma once
#include "../render/NWindowView.h"

#include <SFML/Window/Event.hpp>

using NRawEvent = sf::Event;

struct NWindowEvent {
	NRawEvent rawEvent;
	const NWindowView& viewport;
	const NInputState& input;
};
