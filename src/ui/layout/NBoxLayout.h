#pragma once
#include "NLayout.h"

namespace flx::ui {
	class NVBoxLayout : public NLayout {
	protected:
		NLayoutResult onMeasure(NLayoutConstraint constraint) override;
		void onArrange(rect allocation) override;

	public:
		enum Policy {
			Left,
			Center,
			Right,
			Shrink,
			Fill,
			Top
		};

		Policy alignX = Left;
		Policy widthPolicy = Shrink;
		Policy alignY = Top;

		float maxHeight = 500.f;
	};

	class NHBoxLayout : public NLayout {
	protected:
		NLayoutResult onMeasure(NLayoutConstraint constraint) override;
		void onArrange(rect allocation) override;

	public:
		enum Policy {
			Top,
			Bottom,
			Center
		};

		Policy alignY = Top;
	};
} // namespace flx::ui