#pragma once
#include "NLayout.h"

namespace flx::ui {
	class NKeyValueLayout : public Layout {
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