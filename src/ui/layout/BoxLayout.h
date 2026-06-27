#pragma once
#include "Layout.h"
#include "src/ui/context/Policy.h"

namespace flx::ui {
	class VBoxLayout : public Layout {
	public:
		struct Def {
			Policy alignX = Policy::Left; // Left, Right, Center
			Policy alignY = Policy::Top;  // Top, Center
			Policy widthPolicy = Shrink;  // Shrink, Fill, Fixed
			Policy heightPolicy = Shrink; // Shrink, Fill, Fixed
			vec2 size = {50.f, 50.f};
			Padding padding{};
			float spacing{};
			float maxHeight = 500.f;
		};

		Policy alignX = Policy::Left;		  // Left, Right, Center
		Policy alignY = Policy::Top;		  // Top, Center
		Policy widthPolicy = Policy::Shrink;  // Shrink, Fill, Fixed
		Policy heightPolicy = Policy::Shrink; // Shrink, Fill, Fixed

		float maxHeight = 500.f;

		Measure measure(LayoutConstraint constraint) override;
		void arrange(rect allocation) override;

		static SUnique<VBoxLayout> create(Def def);

		void setSizePolicy(Policy policy) {
			widthPolicy = policy;
			heightPolicy = policy;
		}
	};

	class HBoxLayout : public Layout {
	public:
		struct Def {
			Policy alignY = Top;		  // Top, Bottom, Center
			Policy widthPolicy = Shrink;  // Shrink, Fill, Fixed
			Policy heightPolicy = Shrink; // Shrink, Fill, Fixed
			vec2 size = {50.f, 50.f};
			Padding padding{};
			float spacing{};
		};

		Policy alignY = Top;		  // Top, Bottom, Center
		Policy widthPolicy = Shrink;  // Shrink, Fill, Fixed
		Policy heightPolicy = Shrink; // Shrink, Fill, Fixed

		Measure measure(LayoutConstraint constraint) override;
		void arrange(rect allocation) override;

		static SUnique<HBoxLayout> create(Def def);

		void setSizePolicy(Policy policy) {
			widthPolicy = policy;
			heightPolicy = policy;
		}
	};
} // namespace flx::ui
