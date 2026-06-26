#pragma once
#include "Layout.h"

namespace flx::ui {
	class VBoxLayout : public Layout {
	protected:
		LayoutResult onMeasure(LayoutConstraint constraint) override;
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

		Policy alignX = Left;	// Left, Right, Center
		Policy widthPolicy = Shrink;	// Shink, Fill
		Policy alignY = Top;	// Top, Center

		float maxHeight = 500.f;
	};

	class HBoxLayout : public Layout {
	protected:
		LayoutResult onMeasure(LayoutConstraint constraint) override;
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