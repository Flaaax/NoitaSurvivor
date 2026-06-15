#include "NPanel.h"

#include "src/ui/render/NPainter.h"

#include <SFML/Graphics/RectangleShape.hpp>

namespace flx::ui {
	void NPanel::draw(const NUIPainter& canvas) const {
		sf::RectangleShape shape;
		shape.setFillColor(backgroundColor);
		shape.setOutlineColor(outlineColor);
		shape.setOutlineThickness(outlineThickness);
		shape.setSize(getSize());

		canvas.draw(shape);

		NWidget::draw(canvas);
	}

	void NPanel::setLayout(Unique<NLayout> layout) {
		clear();
		this->addToTop(std::move(layout));
		refresh();
	}

	void NPanel::refresh() {
		const auto layout = getLayout();
		if (sizePolicy == Auto) {
			const vec2 maxSize = {getSize().x, math::inf};
			const vec2 backgroundSize = layout->measure({{}, maxSize}).size;
			layout->arrange({{}, backgroundSize});
			setSize(backgroundSize);
		} else {
			const vec2 backgroundSize = layout->measure({{}, getSize()}).size;
			layout->arrange({{}, backgroundSize});
		}

		visualDirty = false;
	}

	NLayout* NPanel::getLayout() const {
		auto objects = getObjects();
		return static_cast<NLayout*>(objects.empty() ? nullptr : objects.front());
	}
} // namespace flx::ui