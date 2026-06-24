#include "NKeyValueLayout.h"

namespace flx::ui {
	LayoutResult NKeyValueLayout::onMeasure(LayoutConstraint constraint) {
		auto [minSize, maxSize] = constraint;
		float totalHeight{};
		const float maxWidth = maxSize.x - padding.hLength();
		const auto& children = getObjects();

		bool isFirst = true;

		for (u64 i = 0; i < children.size(); i += 2) {
			if (i + 1 >= children.size()) {
				break;
			}
			const auto key = children[i];
			const auto val = children[i + 1];
			const vec2 keySize = key->measure({{}, {maxWidth, math::finf}}).size;
			const vec2 valueSize = val->measure({{}, {maxWidth - keySize.x, math::finf}}).size;
			totalHeight += std::max(keySize.y, valueSize.y);
			if (!isFirst) {
				totalHeight += spacing;
			}
			isFirst = false;
		}

		return {{maxSize.x, totalHeight + padding.vLength()}};
	}

	void NKeyValueLayout::onArrange(rect allocation) {
		setFrame(allocation);

		const float maxWidth = getSize().x - padding.hLength();

		struct Item {
			vec2 keySize{};
			vec2 valueSize{};
			float height{};
			Object* key{};
			Object* value{};
		};

		flx::Vector<Item> items{};
		items.reserve(getObjects().size() / 2);

		const auto& children = getObjects();

		float maxValueWidth{};

		for (u64 i = 0; i < children.size(); i += 2) {
			if (i + 1 >= children.size()) {
				break;
			}
			const auto key = children[i];
			const auto value = children[i + 1];

			const vec2 keySize = key->measure({{}, {maxWidth, math::finf}}).size;
			const vec2 valueSize = value->measure({{}, {maxWidth - keySize.x, math::finf}}).size;

			const float rowHeight = std::max(keySize.y, valueSize.y);
			items.emplace_back(Item{keySize, valueSize, rowHeight, key, value});
			maxValueWidth = std::max(valueSize.x, maxValueWidth);
		}

		float cursorY = padding.top;

		for (const auto& item : items) {
			cursorY += item.height;
			item.key->arrange(rect{{padding.left, cursorY}, item.keySize}.setBottom(cursorY));
			item.value->arrange(rect{{getSize().x - maxValueWidth - padding.right, cursorY}, item.valueSize}.setBottom(cursorY));

			cursorY += spacing;
		}
	}
} // namespace flx::ui