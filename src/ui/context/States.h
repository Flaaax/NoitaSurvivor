#pragma once
#include "../../utils/Vec2/Vec2.h"
#include "src/utils/Container/Vector.h"
#include "src/utils/Pointer.h"

namespace sf {
	class Texture;
}

namespace flx::ui {
	class Layout;
	struct NStyle;

	class Object;

	struct NDragState {
		Object* dragged{};
		vec2 offset{};
	};

	struct NDropCandidate {
		Object* target;
		float score{}; // Higher scrore means higher chance to be se
	};

	struct DropCollector {
		flx::Vector<NDropCandidate> candidates;
	};

	struct DropQuery {
		const NDragState& state;
		const rect globalHitbox;
	};

	struct NHoverState {
		Object* target{};
		float hoveredTime{};
		float hoverIntentDelay{};
		bool hasTargetInFrame{};
		bool mouseMovedInFrame{};
		bool tooltipDirty = true;
	};

	using NTooltipBuilder = Unique<Layout> (*)(const NStyle& style, Object* self);

	struct TooltipSpec {
		NTooltipBuilder builder{};
		float width = 200.f;
		bool shouldUpdate{};
	};
} // namespace flx::ui
