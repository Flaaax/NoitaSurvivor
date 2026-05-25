#pragma once
#include "NDragState.h"
#include "NInputState.h"
#include "src/ui/NScale.h"

#include <SFML/Window/Event.hpp>

using NRawEvent = sf::Event;
class NObject;

struct NEventCtx {
	const NRawEvent& rawEvent;
	const NScale& scale;
	const NInputState& input;
};

struct NWidgetCtx {
	const NDragState& dragState;
};

struct NLocalEventCtx {
	nvec2 mouseLocal;
};

struct NUIEvent {
	const NEventCtx& ctx;
	const NWidgetCtx& widgetCtx;
	NLocalEventCtx localCtx;
};

struct NEventResult {
	struct Pressed {
	};

	struct Clicked {
	};

	struct Dragged {
	};

	// struct DropAccepted {
	// };

	NObject* handler{};
	std::variant<std::monostate, Clicked, Dragged, Pressed> result;

	template <class T>
	bool is() const {
		return std::holds_alternative<T>(result);
	}

	template <class T>
	T* getIf() {
		return std::get_if<T>(result);
	}
};