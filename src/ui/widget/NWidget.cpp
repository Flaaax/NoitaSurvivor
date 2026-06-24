#include "NWidget.h"
#include "src/ui/render/NPainter.h"

#include <ranges>

namespace flx::ui {
	void Widget::bind(Object* obj) {
		assertNotNull(obj);
		assertWithMsg(!obj->parent, "Object already has parent; remove it explicitly first.");
		assertWithMsg(obj != this, "Cannot add a widget to itself!");
		obj->parent = this;
		// obj->root = this->root;
	}

	void Widget::drawWithChildCanvas(const UIPainter& canvas, const Object& obj) {
		const UIPainter localCanvas = canvas.translated(obj.getPosition());
		obj.draw(localCanvas);
	}

	std::optional<NEventResult> Widget::handleEvent(const NUIEvent& event) {
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

	void Widget::update(float deltaTime) {
		if (!updateEnabled)
			return;
		for (const auto& obj : objects) {
			if (obj->updateEnabled && obj->isVisible) {
				obj->update(deltaTime);
			}
		}
	}

	void Widget::draw(const UIPainter& canvas) const {
		for (const auto& obj : objects) {
			if (obj->isVisible && !obj->isDragged()) {
				drawWithChildCanvas(canvas, *obj);
			}
		}
	}

	Unique<Object> Widget::remove(const Object* target) {
		const auto it =
			std::ranges::find_if(
				objects,
				[&](const std::unique_ptr<Object>& obj) {
					return obj.get() == target;
				});
		if (it == objects.end()) {
			logger.warn("Widget does not own target object!");
			return {};
		}
		Unique<Object> removed = std::move(*it);
		removed->parent = {};
		objects.erase(it);
		return removed;
	}

	void Widget::clear() {
		objects = {};
	}

	void Widget::onDropQuery(const DropQuery& query, DropCollector& collector) {
		for (const auto& obj : objects) {
			if (obj->isVisible) {
				obj->onDropQuery(query, collector);
			}
		}
	}

	void Widget::refresh() {
		Object::refresh();
		for (const auto& obj : objects) {
			obj->refresh();
		}
	}
} // namespace flx::ui
