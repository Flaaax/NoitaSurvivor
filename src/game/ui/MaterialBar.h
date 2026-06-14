#pragma once

#include "src/ui/NObject.h"
#include "src/ui/elements/NPanel.h"
#include "src/ui/shapes/NRichTextShape.h"

namespace flx::ui {
	class NRichText;

	class MaterialBar : public ui::NObject {
	private:
		inline static std::string format{};
		int material = 0;
		vec2 rightTop;
		float height;
		mutable ui::NRichTextShape text;

		void updateText();

	public:
		MaterialBar(vec2 rightTop, float height);

		void setData(int material) {
			this->material = material;
			updateText();
		}

		void draw(const ui::NUIPainter& canvas) const override;
	};
} // namespace flx::game