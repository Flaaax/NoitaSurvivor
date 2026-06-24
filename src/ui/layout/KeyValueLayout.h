#pragma once
#include "Layout.h"

namespace flx::ui {
	class KeyValueLayout : public Layout {
	protected:
		LayoutResult onMeasure(LayoutConstraint constraint) override;
		void onArrange(rect allocation) override;

	public:
		enum Policy {
			Left,
			Right,
			Fixed,
			Auto,
			Fill
		};
	};
} // namespace flx::ui