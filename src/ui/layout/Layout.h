#pragma once
#include "src/ui/context/Policy.h"
#include "src/ui/widget/Widget.h"

namespace flx::ui {
	struct Padding {
		float left{};
		float top{};
		float right{};
		float bottom{};

		vec2 size() const {
			return {right + left, top + bottom};
		}

		vec2 leftTop() const {
			return {left, top};
		}

		vec2 rightBottom() const {
			return {right, bottom};
		}

		float hLength() const {
			return right + left;
		}

		float vLength() const {
			return bottom + top;
		}
	};

	class Layout : public Widget {
	protected:
		Padding padding{};
		float spacing{};

	public:
		using enum Policy;

		Measure measure(LayoutConstraint constraint) override = 0;
		void arrange(rect allocation) override = 0;

		void setPadding(Padding value) {
			padding = value;
			// invalidateLayout();
		}

		void setSpacing(float value) {
			spacing = value;
			// invalidateLayout();
		}
	};
} // namespace flx::ui