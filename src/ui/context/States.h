#pragma once
#include "../../utils/Vec2/Vec2.h"
#include "src/utils/Container/Vector.h"

namespace sf {
	class Texture;
}

namespace flx::ui {
	class Layout;
	struct Style;

	class Object;

	struct DragState {
		Ref<Object> dragged{};
		vec2 offset{};
	};

	struct DropCandidate {
		Object* target;
		float score{}; // Higher scrore means higher chance to be se
	};

	struct DropCollector {
		Vector<DropCandidate> candidates;
	};

	struct DropQuery {
		const DragState& state;
		const rect globalHitbox;
	};

	struct HoverState {
		Ref<Object> target{};
		float hoveredTime{};
		float hoverIntentDelay{};
		bool hasTargetInFrame{};
		bool mouseMovedInFrame{};
		bool tooltipDirty = true;
	};

	using TooltipBuilder = SUnique<Layout> (*)(const Style& style, Object* self);

	struct TooltipSpec {
		TooltipBuilder builder{};
		float width = 200.f;
		bool shouldUpdate{};
	};
} // namespace flx::ui
