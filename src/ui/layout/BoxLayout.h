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

		Policy alignX = Left;
		Policy widthPolicy = Shrink;
		Policy alignY = Top;

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