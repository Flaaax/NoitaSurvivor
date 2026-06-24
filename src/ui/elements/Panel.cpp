#include "Panel.h"

#include "src/ui/render/Painter.h"

#include <SFML/Graphics/RectangleShape.hpp>

namespace flx::ui {
	void Panel::draw(const UIPainter& canvas) const {
		sf::RectangleShape shape;
		shape.setFillColor(backgroundColor);
		shape.setOutlineColor(outlineColor);
		shape.setOutlineThickness(outlineThickness);
		shape.setSize(getSize());

		canvas.draw(shape);

		Widget::draw(canvas);
	}

	void Panel::setLayout(Unique<Layout> layout) {
		clear();
		this->addToTop(std::move(layout));
		refresh();
	}

	void Panel::refresh() {
		const auto layout = getLayout();
		if (sizePolicy == Auto) {
			const vec2 maxSize = {getSize().x, math::finf};
			const vec2 backgroundSize = layout->measure({{}, maxSize}).size;
			layout->arrange({{}, backgroundSize});
			setSize(backgroundSize);
		} else {
			const vec2 backgroundSize = layout->measure({{}, getSize()}).size;
			layout->arrange({{}, backgroundSize});
		}

		visualDirty = false;
	}

	Layout* Panel::getLayout() const {
		auto objects = getObjects();
		return static_cast<Layout*>(objects.empty() ? nullptr : objects.front());
	}
} // namespace flx::ui