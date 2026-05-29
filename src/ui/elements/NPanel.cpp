#include "NPanel.h"

#include "src/ui/render/NCanvas.h"

#include <SFML/Graphics/RectangleShape.hpp>

void NPanel::draw(const NCanvas& canvas) const {
	sf::RectangleShape shape;
	shape.setFillColor(backgroundColor);
	shape.setOutlineColor(outlineColor);
	shape.setOutlineThickness(outlineThickness);
	shape.setSize(getSize());

	canvas.draw(shape);

	NWidget::draw(canvas);
}

void NPanel::setLayout(n_unique<NLayout> layout) {
	clear();
	const auto layout_ = layout.get();
	this->addToTop(std::move(layout));
	nvec2 backgroundSize = getSize();
	const nvec2 maxSize = {getSize().x, nmath::inf};
	backgroundSize = layout_->measure({{}, maxSize}).size;
	layout_->arrange({{}, backgroundSize});
	setSize(backgroundSize);
	visualDirty = false;
}

NObject* NPanel::getLayout() const {
	auto objects = getObjects();
	return objects.empty() ? nullptr : objects.front();
}