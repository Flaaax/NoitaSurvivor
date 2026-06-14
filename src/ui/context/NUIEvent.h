#pragma once
#include "NInputState.h"
#include "NWindowEvent.h"
#include "States.h"
#include <SFML/Window/Event.hpp>

namespace flx::ui {
	class NObject;

	struct NRootCtx {
		const NDragState& dragState;
	};

	struct NLocalEventCtx {
		vec2 mouseLocal;
	};

	struct NUIEvent {
		const NWindowEvent& windowEvent;
		const NRootCtx& rootCtx;
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
} // namespace flx::ui
