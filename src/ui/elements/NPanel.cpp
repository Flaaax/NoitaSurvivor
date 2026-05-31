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
	this->addToTop(std::move(layout));
	refreshLayout();
}

void NPanel::refreshLayout() {
	const auto layout = getLayout();
	if (sizePolicy == Auto) {
		const nvec2 maxSize = {getSize().x, nmath::inf};
		const nvec2 backgroundSize = layout->measure({{}, maxSize}).size;
		layout->arrange({{}, backgroundSize});
		setSize(backgroundSize);
	} else {
		const nvec2 backgroundSize = layout->measure({{}, getSize()}).size;
		layout->arrange({{}, backgroundSize});
	}

	visualDirty = false;
}

NLayout* NPanel::getLayout() const {
	auto objects = getObjects();
	return static_cast<NLayout*>(objects.empty() ? nullptr : objects.front());
}