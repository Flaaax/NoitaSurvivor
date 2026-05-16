#pragma once

class NObject;

struct NDragState {
	NObject* dragged{};
	nvec2 offset{};
};