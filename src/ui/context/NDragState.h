#pragma once
#include "../../utils/Vec2/Vec2.h"
#include "src/utils/Container/Vector.h"

class NObject;

struct NDragState {
	NObject* dragged{};
	nvec2 offset{};
};

struct NDropCandidate{
	NObject* target;
	float score{};		//Higher scrore means higher chance to be se
};

struct NDropCollector {
	Util::Vector<NDropCandidate> candidates;
};

struct NDropQuery {
	const NDragState& state;
	const nrect globalHitbox;
};