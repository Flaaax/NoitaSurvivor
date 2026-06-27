#pragma once
#include "Layout.h"

namespace flx::ui {
	class KeyValueLayout : public Layout {
	public:
		Measure measure(LayoutConstraint constraint) override;
		void arrange(rect allocation) override;
	};
} // namespace flx::ui