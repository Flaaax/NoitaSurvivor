#pragma once
#include "Layout.h"

namespace flx::ui {
	class VBoxLayout : public Layout {
	protected:
		LayoutResult onMeasure(LayoutConstraint constraint) override;
		void onArrange(rect allocation) override;

	public:
		Policy alignX = Left;          // Left, Right, Center
		Policy alignY = Top;           // Top, Center
		Policy widthPolicy = Shrink;   // Shrink, Fill, Fixed
		Policy heightPolicy = Shrink;  // Shrink, Fill, Fixed

		float maxHeight = 500.f;
	};

	class HBoxLayout : public Layout {
	protected:
		LayoutResult onMeasure(LayoutConstraint constraint) override;
		void onArrange(rect allocation) override;

	public:
		Policy alignY = Top;           // Top, Bottom, Center
		Policy widthPolicy = Shrink;   // Shrink, Fill, Fixed
		Policy heightPolicy = Shrink;  // Shrink, Fill, Fixed
	};
} // namespace flx::ui
