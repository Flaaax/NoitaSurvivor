#pragma once
#include "NLayout.h"

class NKeyValueLayout : public NLayout {
protected:
	NLayoutResult onMeasure(NLayoutConstraint constraint) override;
	void onArrange(nrect allocation) override;

public:
	enum Policy {
		Left,
		Right,
		Fixed,
		Auto,
		Fill
	};
};