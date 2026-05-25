#pragma once
#include "shapes/NRichTextShape.h"
#include "src/utils/Pointer.h"
#include "widget/NWidget.h"

class NToolTip : public NObject {
private:
	n_unique<sf::Sprite> sprite{};
	Util::Vector<NRichTextShape> textLines{};

	void updateLayout();

public:
	void set(Util::Vector<NRichTextShape> textLines, n_unique<sf::Sprite> sprite = {});
	void draw(const NCanvas& canvas) const override;

};