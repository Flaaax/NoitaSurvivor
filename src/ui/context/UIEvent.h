#pragma once
#include "InputState.h"
#include "States.h"
#include "WindowEvent.h"
#include <SFML/Window/Event.hpp>

namespace flx::ui {
	class Object;

	struct RootCtx {
		const DragState& dragState;
	};

	struct LocalEventCtx {
		vec2 mouseLocal;
	};

	struct UIEvent {
		const WindowEvent& windowEvent;
		const RootCtx& rootCtx;
		LocalEventCtx localCtx;
	};

	struct EventResult {
		struct Pressed {
		};

		struct Clicked {
		};

		struct DragIntent {
		};

		struct HoverIntent {
		};

		Object* handler{};
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
