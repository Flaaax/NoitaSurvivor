#pragma once
#include "context/NStyle.h"
#include "shapes/NRichTextShape.h"
#include "widget/NWidget.h"

class NTooltip : public NObject {
private:
	mutable std::optional<sf::Sprite> sprite{};
	mutable Util::Vector<NRichTextShape> textLines{};
	bool hasTitle{};

	void updateLayout();

public:
	void setContent(const NTooltipSpec& spec, const NTooltipStyle& style);
	void draw(const NCanvas& canvas) const override;
};