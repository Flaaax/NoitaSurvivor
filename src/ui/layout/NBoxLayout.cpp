#include "NBoxLayout.h"

namespace flx::ui {
	NLayoutResult NVBoxLayout::onMeasure(NLayoutConstraint constraint) {
		// TODO minSize ignored
		auto [minSize, maxSize] = constraint;

		maxSize.y = std::min(maxSize.y, maxHeight);
		const auto contentMaxSize = maxSize - padding.size();

		const NLayoutConstraint childConstraint{
			.minSize = {},
			.maxSize = {contentMaxSize.x, math::inf},
		};

		vec2 layout{};

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

		if (alignY == Center) {
			layout.y = maxSize.y;
		}

		if (widthPolicy == Fill) {
			return {{maxSize.x, layout.y + padding.vLength()}};
		}

		// Fallback Policy: Shrink
		return {layout + padding.size()};
	}

	void NVBoxLayout::onArrange(rect allocation) {
		auto [pos, size] = allocation;
		const auto contentSize = size - padding.size();

		const NLayoutConstraint childConstraint{
			.minSize = {},
			.maxSize = {contentSize.x, math::inf},
		};

		bool first = true;

		struct ChildLayout {
			float y{};
			vec2 size{};
			NObject* child{};
		};

		flx::Vector<ChildLayout> children{};
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

		const float yTotal = cursorY - padding.top;
		cursorY = padding.top + contentSize.y / 2.f - yTotal / 2.f;

		for (auto [y, size, child] : children) {
			rect arrange = {{padding.left, y}, size};
			if (alignX == Left) {
				// pass
			} else if (alignX == Right) {
				arrange.setRight(padding.left + contentSize.x);
			} else if (alignX == Center) {
				arrange.setXCenter(padding.left + contentSize.x / 2.f);
			}

			if (alignY == Center) {
				arrange.position.y = cursorY;
				cursorY += size.y;
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
			.maxSize = {math::inf, contentMaxSize.y},
		};

		vec2 layout{};

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

	void NHBoxLayout::onArrange(rect allocation) {
		auto [pos, size] = allocation;
		const auto contentSize = size - padding.size();

		const NLayoutConstraint childConstraint{
			.minSize = {},
			.maxSize = {math::inf, contentSize.y},
		};

		struct ChildLayout {
			float x{};
			vec2 size{};
			NObject* child{};
		};

		flx::Vector<ChildLayout> children{};
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
			rect arrange = {{x, padding.top}, childSize};

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
} // namespace flx::ui