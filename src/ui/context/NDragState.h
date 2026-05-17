#pragma once
#include "src/utils/Container/Vector.h"
#include "src/utils/Vec2.h"

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