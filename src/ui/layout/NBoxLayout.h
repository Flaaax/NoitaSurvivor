#pragma once
#include "NLayout.h"

class NVBoxLayout : public NLayout {
protected:
	NLayoutResult onMeasure(NLayoutConstraint constraint) override;
	void onArrange(nrect allocation) override;

public:
	enum Policy {
		Left,
		Center,
		Right
	};

	Policy alignX = Left;
};

class NHBoxLayout : public NLayout {
protected:
	NLayoutResult onMeasure(NLayoutConstraint constraint) override;
	void onArrange(nrect allocation) override;

public:
	enum Policy {
		Top,
		Bottom,
		Center
	};

	Policy alignY = Top;
};