#pragma once
#include "NInputState.h"
#include "States.h"
#include "src/ui/NScale.h"

#include <SFML/Window/Event.hpp>

using NRawEvent = sf::Event;
class NObject;

struct NEventCtx {
	const NRawEvent& rawEvent;
	const NScale& scale;
	const NInputState& input;
};

struct NRootCtx {
	const NDragState& dragState;
};

struct NLocalEventCtx {
	nvec2 mouseLocal;
};

struct NUIEvent {
	const NEventCtx& ctx;
	const NRootCtx& widgetCtx;
	NLocalEventCtx localCtx;
};

struct NEventResult {
	struct Pressed {
	};

	struct Clicked {
	};

	struct DragIntent {
	};

	struct HoverIntent {
	};

	NObject* handler{};
	std::variant<std::monostate, Clicked, DragIntent, Pressed, HoverIntent> result;

	template <class T>
	bool is() const {
		return std::holds_alternative<T>(result);
	}

	template <class T>
	T* getIf() {
		return std::get_if<T>(result);
	}
};