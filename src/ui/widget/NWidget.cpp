#include "NWidget.h"
#include "../NWindow.h"
#include "../Renderer.h"

#include <ranges>

std::optional<NEventResult> NWidget::handleEvent(const NEvent& event) {
	for (const auto& obj : objects | std::views::reverse) {
		if (!obj->isVisible) {
			continue;
		}
		NEvent localEvent = event;
		localEvent.localCtx.mouseLocal = event.localCtx.mouseLocal - obj->getPosition();
		if (const auto result = obj->handleEvent(localEvent)) {
			return result;
		}
	}
	return std::nullopt;
}

bool NWidget::handleEvent(const NEventCtx& ctx) {
	const NEvent event{
		.ctx = ctx,
		.widgetCtx = widgetCtx};
	const auto result = this->handleEvent(event);

	if (!result) {
		return false;
	}

	if (result->is<NEventResult::Dragged>()) {
		NObject* handler = result->handler;
		widgetCtx.dragState.dragged = handler;
		widgetCtx.dragState.offset = ctx.input.mouseRender - handler->getGlobalPosition();
	} else if (result->is<NEventResult::DropAccepted>()) {
		if (!result->handler->isWidget()) {
			Logger::warn("Non-widget object cannot accpet drop!");
		} else {
			NWidget* handler = result->handler->asWidget();
			handler->onDropAccepted(widgetCtx.dragState);
		}
		widgetCtx.dragState.dragged = {};
	}

	return true;
}

void NWidget::update(float deltaTime) {
	if (!updateEnabled)
		return;
	for (const auto& obj : objects) {
		if (obj->updateEnabled && obj->isVisible()) {
			obj->update(deltaTime);
		}
	}
}

void NWidget::draw(Renderer& renderer) const {
	/*sf::RectangleShape shape;
	shape.setSize(m_geometry.size());
	shape.setPosition(renderPos);
	shape.setFillColor(sf::Color(200, 200, 200));
	renderer.drawGui(shape);*/

	renderer.guiContentState.transform.translate(this->m_geometry.position);

	for (const auto& obj : objects) {
		if (obj->isVisible()) {
			obj->draw(renderer);
		}
	}

	renderer.guiContentState.transform.translate(-this->m_geometry.position);
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
	std::unique_ptr<NObject> removed = std::move(*it);
	removed->parent = {};
	objects.erase(it);
	return removed;
}