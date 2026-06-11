#pragma once

#include "src/ui/NObject.h"
#include "src/ui/elements/NPanel.h"
#include "src/ui/shapes/NRichTextShape.h"

class NRichText;

class MaterialBar : public NObject {
private:
	inline static std::string format{};
	int material = 0;
	nvec2 rightTop;
	float height;
	mutable NRichTextShape text;

	void updateText();

public:
	MaterialBar(nvec2 rightTop, float height);

	void setData(int material) {
		this->material = material;
		updateText();
	}

	void draw(const NUIPainter& canvas) const override;
};