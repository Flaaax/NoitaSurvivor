#include "Panel.h"

#include "src/ui/render/Painter.h"

#include <SFML/Graphics/RectangleShape.hpp>

namespace flx::ui {
	SUnique<Panel> Panel::create(Def def) {
		auto ret = makeSUnique<Panel>();
		ret->sizePolicy = def.sizePolicy;
		ret->setSize(def.size);
		ret->outlineThickness = def.outlineThickness;
		ret->outlineColor = def.outlineColor;
		ret->backgroundColor = def.backgroundColor;
		ret->setLayout(def.layout | move);
		return ret;
	}

	void Panel::draw(const UIPainter& canvas) const {
		sf::RectangleShape shape;
		shape.setFillColor(backgroundColor);
		shape.setOutlineColor(outlineColor);
		shape.setOutlineThickness(outlineThickness);
		shape.setSize(getSize());

		canvas.draw(shape);

		Widget::draw(canvas);
	}

	void Panel::setLayout(SUnique<Layout> layout) {
		clear();
		this->layout = layout;
		this->addToTop(std::move(layout));
		refresh();
	}

	const Layout* Panel::getLayout() const {
		return layout.get();
	}

	void Panel::refresh() {
		visualDirty = false;

		if (!layout) {
			return;
		}

		if (sizePolicy == Policy::Shrink) {
			const vec2 backgroundSize = layout->measure({{}, getSize()}).size;
			layout->arrange({{}, backgroundSize});
			setSize(backgroundSize);
		} else if (sizePolicy == Policy::Fixed) {
			layout->arrange({{}, getSize()});
		} else {
			logger.error_and_throw("Invalid policy");
		}
	}

	Measure Panel::measure(LayoutConstraint constraint) {
		if (sizePolicy == Policy::Fixed) {
			return {getSize()};
		}
		if (sizePolicy == Policy::Shrink) {
			if (!layout) {
				return {};
			}
			return layout->measure(constraint);
		}
		logger.error_and_throw("Invalid policy");
	}

	void Panel::arrange(rect rect) {
		setPosition(rect.position);
		if (sizePolicy == Policy::Shrink) {
			setSize(rect.size);
		}

		refresh();
	}

	// Layout* Panel::getLayout() const {
	// 	auto objects = getObjects();
	// 	if (objects.empty()) {
	// 		return {};
	// 	}
	// 	return objects.empty() ? nullptr : static_cast<Layout*>(objects.front());
	// }
} // namespace flx::ui