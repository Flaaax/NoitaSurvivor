#pragma once

#include "../Object.h"
#include "NPanel.h"
#include <SFML/Graphics/Text.hpp>
#include <functional>

namespace flx::ui {
	class NRichText;

	class NButton : public NPanel {
	public:
		std::function<void()> onClick;

		NRichText* text{};

		bool isButtonVisible = true;
		bool isActuallyPressed{};

		enum ButtonState {
			Hovered,
			Normal,
			Pressed
		} state = Normal;

		NButton() : NButton({0, 0, 50, 50}) {
		}

		explicit NButton(rect geometry);

		std::optional<NEventResult> handleEvent(const NUIEvent& event) override;
		void draw(const UIPainter& canvas) const override;

		void setText(std::string_view text, u32 characterSize = 30);

		void setOnClick(const std::function<void()>& func) {
			onClick = func;
		}
	};
}

