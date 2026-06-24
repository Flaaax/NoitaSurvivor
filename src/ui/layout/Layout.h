#pragma once
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

		virtual LayoutResult onMeasure(LayoutConstraint constraint) = 0;
		virtual void onArrange(rect allocation) = 0;

	public:
		LayoutResult measure(LayoutConstraint constraint) final {
			return onMeasure(constraint);
		}

		void arrange(rect allocation) final {
			setPosition(allocation.position);
			onArrange(allocation);
		}

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