#include "NKeyValueLayout.h"

NLayoutResult NKeyValueLayout::onMeasure(NLayoutConstraint constraint) {
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
		const nvec2 keySize = key->measure({{}, {maxWidth, nmath::inf}}).size;
		const nvec2 valueSize = val->measure({{}, {maxWidth - keySize.x, nmath::inf}}).size;
		totalHeight += std::max(keySize.y, valueSize.y);
		if (!isFirst) {
			totalHeight += spacing;
		}
		isFirst = false;
	}

	return {{maxSize.x, totalHeight + padding.vLength()}};
}

void NKeyValueLayout::onArrange(nrect allocation) {
	setFrame(allocation);

	const float maxWidth = getSize().x - padding.hLength();

	struct Item {
		float keyWidth{};
		float valueWidth{};
		float height{};
		NObject* key{};
		NObject* value{};
	};

	Util::Vector<Item> items{};
	items.reserve(getObjects().size() / 2);

	const auto& children = getObjects();

	float maxValueWidth{};

	for (u64 i = 0; i < children.size(); i += 2) {
		if (i + 1 >= children.size()) {
			break;
		}
		const auto key = children[i];
		const auto value = children[i + 1];

		const nvec2 keySize = key->measure({{}, {maxWidth, nmath::inf}}).size;
		const nvec2 valueSize = value->measure({{}, {maxWidth - keySize.x, nmath::inf}}).size;

		const float rowHeight = std::max(keySize.y, valueSize.y);
		items.emplace_back(Item{keySize.x, valueSize.x, rowHeight, key, value});
		maxValueWidth = std::max(valueSize.x, maxValueWidth);
	}

	float cursorY = padding.top;

	for (const auto& item : items) {
		item.key->arrange({{padding.left, cursorY}, {item.keyWidth, item.height}});
		item.value->arrange({{getSize().x - maxValueWidth - padding.right, cursorY}, {item.valueWidth, item.height}});

		cursorY += spacing + item.height;
	}
}