#pragma once
#include "Layout.h"

namespace flx::ui {
	class VBoxLayout : public Layout {
	protected:
		LayoutResult onMeasure(LayoutConstraint constraint) override;
		void onArrange(rect allocation) override;

	public:
		Policy alignX = Left;		 // Left, Right, Center
		Policy alignY = Top;		 // Top, Center
		Policy widthPolicy = Shrink; // Shink, Fill

		float maxHeight = 500.f;
	};

	class HBoxLayout : public Layout {
	protected:
		LayoutResult onMeasure(LayoutConstraint constraint) override;
		void onArrange(rect allocation) override;

	public:
		Policy alignY = Top;
	};
} // namespace flx::ui