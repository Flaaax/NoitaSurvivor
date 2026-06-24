#pragma once

#include "src/ui/Object.h"
#include "src/ui/elements/Panel.h"
#include "src/ui/shapes/RichTextShape.h"

namespace flx::ui {
	class RichText;

	class MaterialBar : public ui::Object {
	private:
		inline static std::string format{};
		int material = 0;
		vec2 rightTop;
		float height;
		mutable ui::RichTextShape text;

		void updateText();

	public:
		MaterialBar(vec2 rightTop, float height);

		void setData(int material) {
			this->material = material;
			updateText();
		}

		void draw(const ui::UIPainter& canvas) const override;
	};
} // namespace flx::game