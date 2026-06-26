#include "BoxLayout.h"

#include <algorithm>

namespace flx::ui {
	namespace {
		vec2 contentSizeOf(vec2 size, Padding padding) {
			auto contentSize = size - padding.size();
			contentSize.x = std::max(0.f, contentSize.x);
			contentSize.y = std::max(0.f, contentSize.y);
			return contentSize;
		}
	} // namespace

	LayoutResult VBoxLayout::onMeasure(LayoutConstraint constraint) {
		// TODO minSize ignored
		auto [minSize, maxSize] = constraint;

		if (widthPolicy == Fixed) {
			maxSize.x = getSize().x;
		}
		if (heightPolicy == Fixed) {
			maxSize.y = getSize().y;
		} else {
			maxSize.y = std::min(maxSize.y, maxHeight);
		}

		const auto contentMaxSize = contentSizeOf(maxSize, padding);
		const LayoutConstraint childConstraint{
			.minSize = {},
			.maxSize = {contentMaxSize.x, math::finf},
		};

		vec2 layout{};
		bool isFirst = true;

		for (const auto child : getObjects()) {
			auto childSize = child->measure(childConstraint).size;

			layout.x = std::max(layout.x, childSize.x);

			if (!isFirst) {
				layout.y += spacing;
			}

			layout.y += childSize.y;
			isFirst = false;
		}

		auto measuredSize = layout + padding.size();

		if (widthPolicy == Fill || widthPolicy == Fixed) {
			measuredSize.x = maxSize.x;
		}
		if (heightPolicy == Fill || heightPolicy == Fixed) {
			measuredSize.y = maxSize.y;
		}

		return {measuredSize};
	}

	void VBoxLayout::onArrange(rect allocation) {
		auto [pos, size] = allocation;
		const auto contentSize = contentSizeOf(size, padding);

		const LayoutConstraint childConstraint{
			.minSize = {},
			.maxSize = {contentSize.x, math::finf},
		};

		struct ChildLayout {
			float y{};
			vec2 size{};
			Object* child{};
		};

		Vector<ChildLayout> children{};
		children.reserve(getObjects().size());

		float cursorY = padding.top;
		bool first = true;

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

		const float contentHeight = cursorY - padding.top;
		const float centeredTop = padding.top + contentSize.y / 2.f - contentHeight / 2.f;

		for (auto [y, childSize, child] : children) {
			if (alignY == Center) {
				y = centeredTop + (y - padding.top);
			}

			rect arrange = {{padding.left, y}, childSize};
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

	LayoutResult HBoxLayout::onMeasure(LayoutConstraint constraint) {
		// TODO minSize ignored
		auto [minSize, maxSize] = constraint;

		if (widthPolicy == Fixed) {
			maxSize.x = getSize().x;
		}
		if (heightPolicy == Fixed) {
			maxSize.y = getSize().y;
		}

		const auto contentMaxSize = contentSizeOf(maxSize, padding);
		const LayoutConstraint childConstraint{
			.minSize = {},
			.maxSize = {math::finf, contentMaxSize.y},
		};

		vec2 layout{};
		bool isFirst = true;

		for (const auto child : getObjects()) {
			auto childSize = child->measure(childConstraint).size;

			layout.y = std::max(layout.y, childSize.y);

			if (!isFirst) {
				layout.x += spacing;
			}

			layout.x += childSize.x;
			isFirst = false;
		}

		auto measuredSize = layout + padding.size();

		if (widthPolicy == Fill || widthPolicy == Fixed) {
			measuredSize.x = maxSize.x;
		}
		if (heightPolicy == Fill || heightPolicy == Fixed) {
			measuredSize.y = maxSize.y;
		}

		return {measuredSize};
	}

	void HBoxLayout::onArrange(rect allocation) {
		auto [pos, size] = allocation;
		const auto contentSize = contentSizeOf(size, padding);

		const LayoutConstraint childConstraint{
			.minSize = {},
			.maxSize = {math::finf, contentSize.y},
		};

		struct ChildLayout {
			float x{};
			vec2 size{};
			Object* child{};
		};

		Vector<ChildLayout> children{};
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
				// pass
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
