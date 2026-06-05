#pragma once
#include "../render/NWindowViewport.h"

#include <SFML/Window/Event.hpp>

using NRawEvent = sf::Event;

struct NWindowEvent {
	NRawEvent rawEvent;
	const NWindowViewport& viewport;
	const NInputState& input;
};
