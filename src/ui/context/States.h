#pragma once
#include "../../utils/Vec2/Vec2.h"
#include "src/utils/Container/Vector.h"
#include "src/utils/Integers.h"

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

struct NTooltipSpec {
	const sf::Texture* iconTexture{};
	nvec2 iconSize{};

	std::string title{};
	Util::Vector<std::string> contents;
	std::string flavor{};

	bool shouldUpdate{};
};