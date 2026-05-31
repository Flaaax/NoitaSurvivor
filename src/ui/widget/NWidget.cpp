#include "NWidget.h"
#include "../NWindow.h"
#include "src/ui/render/NCanvas.h"

#include <ranges>

void NWidget::bind(NObject* obj) {
	assertNotNull(obj);
	assertWithMsg(!obj->parent, "Object already has parent; remove it explicitly first.");
	assertWithMsg(obj != this, "Cannot add a widget to itself!");
	obj->parent = this;
}

void NWidget::drawWithChildCanvas(const NCanvas& canvas, const NObject& obj) {
	const NCanvas localCanvas = canvas.translated(obj.getPosition());
	obj.draw(localCanvas);
}

std::optional<NEventResult> NWidget::handleEvent(const NUIEvent& event) {
	NUIEvent localEvent = event;
	localEvent.localCtx.mouseLocal = event.localCtx.mouseLocal - getPosition();
	for (const auto& obj : objects | std::views::reverse) {
		if (!obj->isVisible) {
			continue;
		}
		if (const auto result = obj->handleEvent(localEvent)) {
			return result;
		}
	}
	return std::nullopt;
}

void NWidget::update(float deltaTime) {
	if (!updateEnabled)
		return;
	for (const auto& obj : objects) {
		if (obj->updateEnabled && obj->isVisible) {
			obj->update(deltaTime);
		}
	}
}

void NWidget::draw(const NCanvas& canvas) const {
	for (const auto& obj : objects) {
		if (obj->isVisible && !obj->isDragged()) {
			drawWithChildCanvas(canvas, *obj);
		}
	}
}

n_unique<NObject> NWidget::remove(const NObject* target) {
	const auto it =
		std::ranges::find_if(
			objects,
			[&](const std::unique_ptr<NObject>& obj) {
				return obj.get() == target;
			});
	if (it == objects.end()) {
		Logger::warn("Widget does not own target object!");
		return {};
	}
	n_unique<NObject> removed = std::move(*it);
	removed->parent = {};
	objects.erase(it);
	return removed;
}

void NWidget::clear() {
	objects = {};
}

void NWidget::onDropQuery(const NDropQuery& query, NDropCollector& collector) {
	for (const auto& obj : objects) {
		if (obj->isVisible) {
			obj->onDropQuery(query, collector);
		}
	}
}
