#pragma once

#include "../Object.h"
#include "Panel.h"
#include <functional>

namespace flx::ui {
	class RichText;

	class Button : public Panel {
	public:
		std::function<void()> onClick;

		Ref<RichText> text{};

		bool hasBackground = true;
		bool isActuallyPressed{};

		enum ButtonState {
			Hovered,
			Normal,
			Pressed
		} state = Normal;

		Button()
			: Button({0, 0, 50, 50}) {
		}

		explicit Button(rect geometry);

		// static SUnique<Button> create(rect geometry, std::string text, u32 characterSize, std::function<void> onClick = {});

		std::optional<EventResult> handleEvent(const UIEvent& event) override;
		void draw(const UIPainter& canvas) const override;

		void setText(std::string text, u32 characterSize = 30);

		void setOnClick(std::function<void()> func) {
			onClick = func | move;
		}
	};
} // namespace flx::ui
