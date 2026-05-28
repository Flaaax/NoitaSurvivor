#include "NBoxLayout.h"

NLayoutResult NVBoxLayout::onMeasure(NLayoutConstraint constraint) {
	// TODO minSize ignored
	auto [minSize, maxSize] = constraint;
	const auto contentMaxSize = maxSize - padding.size();

	const NLayoutConstraint childConstraint{
		.minSize = {},
		.maxSize = {contentMaxSize.x, nmath::inf},
	};

	nvec2 layout{};

	bool isFirst = true;

	for (const auto child : getObjects()) {
		auto childSize = child->measure(childConstraint).size;

		// Change width dynamically
		layout.x = std::max(layout.x, childSize.x);

		if (!isFirst) {
			layout.y += spacing;
		}

		layout.y += childSize.y;

		isFirst = false;
	}

	return {layout + padding.size()};
}

void NVBoxLayout::onArrange(nrect allocation) {
	auto [pos, size] = allocation;
	const auto contentSize = size - padding.size();

	const NLayoutConstraint childConstraint{
		.minSize = {},
		.maxSize = {contentSize.x, nmath::inf},
	};

	bool first = true;

	struct ChildLayout {
		float y{};
		nvec2 size{};
		NObject* child{};
	};

	Util::Vector<ChildLayout> children{};
	children.reserve(getObjects().size());
	float cursorY = padding.top;

	for (const auto child : getObjects()) {
		const auto childSize = child->measure(childConstraint).size;

		if (!first) {
			cursorY += spacing;
		}

		children += ChildLayout{
			.y = cursorY,
			.size = childSize,
			.child = child,
		};

		cursorY += childSize.y;

		first = false;
	}

	for (auto [y, size, child] : children) {
		nrect arrange = {{padding.left, y}, size};
		if (alignX == Left) {
			// pass
		} else if (alignX == Right) {
			arrange.setRight(padding.left + contentSize.x);
		} else if (alignX == Center) {
			arrange.setXCenter(padding.left + contentSize.x / 2.f);
		}
		child->arrange(arrange);
	}

	setFrame(allocation);
}

NLayoutResult NHBoxLayout::onMeasure(NLayoutConstraint constraint) {
	// TODO minSize ignored

	auto [minSize, maxSize] = constraint;
	const auto contentMaxSize = maxSize - padding.size();

	const NLayoutConstraint childConstraint{
		.minSize = {},
		.maxSize = {nmath::inf, contentMaxSize.y},
	};

	nvec2 layout{};

	bool isFirst = true;

	for (const auto child : getObjects()) {
		auto childSize = child->measure(childConstraint).size;

		// Change height dynamically
		layout.y = std::max(layout.y, childSize.y);

		if (!isFirst) {
			layout.x += spacing;
		}

		layout.x += childSize.x;

		isFirst = false;
	}

	return {layout + padding.size()};
}

void NHBoxLayout::onArrange(nrect allocation) {
	auto [pos, size] = allocation;
	const auto contentSize = size - padding.size();

	const NLayoutConstraint childConstraint{
		.minSize = {},
		.maxSize = {nmath::inf, contentSize.y},
	};

	struct ChildLayout {
		float x{};
		nvec2 size{};
		NObject* child{};
	};

	Util::Vector<ChildLayout> children{};
	children.reserve(getObjects().size());

	float cursorX = padding.left;
	bool first = true;

	for (const auto child : getObjects()) {
		const auto childSize = child->measure(childConstraint).size;

		if (!first) {
			cursorX += spacing;
		}

		children += ChildLayout{
			.x = cursorX,
			.size = childSize,
			.child = child,
		};

		cursorX += childSize.x;
		first = false;
	}

	for (auto [x, childSize, child] : children) {
		nrect arrange = {{x, padding.top}, childSize};

		if (alignY == Top) {
			// Pass
		} else if (alignY == Bottom) {
			arrange.setBottom(padding.top + contentSize.y);
		} else if (alignY == Center) {
			arrange.setYCenter(padding.top + contentSize.y / 2.f);
		}

		child->arrange(arrange);
	}

	setFrame(allocation);
}