#pragma once

#include "../Object.h"
#include "Panel.h"
#include <SFML/Graphics/Text.hpp>
#include <functional>

namespace flx::ui {
	class RichText;

	class Button : public Panel {
	public:
		std::function<void()> onClick;

		RichText* text{};

		bool isButtonVisible = true;
		bool isActuallyPressed{};

		enum ButtonState {
			Hovered,
			Normal,
			Pressed
		} state = Normal;

		Button() : Button({0, 0, 50, 50}) {
		}

		explicit Button(rect geometry);

		std::optional<EventResult> handleEvent(const UIEvent& event) override;
		void draw(const UIPainter& canvas) const override;

		void setText(std::string_view text, u32 characterSize = 30);

		void setOnClick(const std::function<void()>& func) {
			onClick = func;
		}
	};
}

