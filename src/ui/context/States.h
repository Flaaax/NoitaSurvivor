#pragma once
#include "../../utils/Vec2/Vec2.h"
#include "src/utils/Container/Vector.h"
#include "src/utils/Pointer.h"

class NLayout;
struct NStyle;

namespace sf {
	class Texture;
}
class NObject;

struct NDragState {
	NObject* dragged{};
	nvec2 offset{};
};

struct NDropCandidate {
	NObject* target;
	float score{}; // Higher scrore means higher chance to be se
};

struct NDropCollector {
	Util::Vector<NDropCandidate> candidates;
};

struct NDropQuery {
	const NDragState& state;
	const nrect globalHitbox;
};

struct NHoverState {
	NObject* target{};
	float hoveredTime{};
	float hoverIntentDelay{};
	bool hasTargetInFrame{};
	bool mouseMovedInFrame{};
	bool tooltipDirty = true;
};

using NTooltipBuilder = n_unique<NLayout> (*)(const NStyle& style, NObject* self);

struct NTooltipSpec {
	NTooltipBuilder builder{};
	float width = 200.f;
	bool shouldUpdate{};
};