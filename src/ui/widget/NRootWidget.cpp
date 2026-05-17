#include "NRootWidget.h"

bool NRootWidget::handleDragEvent(const NEventCtx& event) {
	if (event.rawEvent.is<sf::Event::MouseMoved>()) {
		const nvec2 newGlobalPosition = event.input.mouseRender - dragState.offset;
		const nvec2 newLocalPosition = dragState.dragged->getLocalPosition(newGlobalPosition);
		dragState.dragged->setPosition(newLocalPosition);
	}

	const NDropQuery query{
		.state = dragState,
		.globalHitbox = dragState.dragged->getGlobalHitbox(),
	};
	NDropCollector collector;
	onDropQuery(query, collector);
	NObject* candidate{};
	if (!collector.candidates.empty()) {
		constexpr auto compare = [](const NDropCandidate& a, const NDropCandidate& b) {
			return a.score > b.score;
		};
		candidate = collector.candidates.best(compare).target;
	}

	const auto e = event.rawEvent.getIf<sf::Event::MouseButtonReleased>();
	const bool shouldDrop = e && e->button == sf::Mouse::Button::Left;

	if (candidate) {
		candidate->onDropAccepted(query, shouldDrop);
	}

	if (shouldDrop) {
		dragState.dragged->isDragged_ = false;
		dragState.dragged = {};
		return true;
	}

	return false;
}

bool NRootWidget::handleEvent(const NEventCtx& ctx) {
	const NWidgetCtx widgetCtx{
		.dragState = dragState,
	};
	const NUIEvent event{
		.ctx = ctx,
		.widgetCtx = widgetCtx,
		.localCtx = NLocalEventCtx{
			.mouseLocal = ctx.input.mouseRender,
		},
	};

	if (widgetCtx.dragState.dragged && handleDragEvent(ctx)) {
		return true;
	}

	const auto result = this->NWidget::handleEvent(event);

	if (!result) {
		return false;
	}

	if (result->is<NEventResult::Dragged>()) {
		NObject* handler = result->handler;
		dragState.dragged = handler;
		dragState.offset = ctx.input.mouseRender - handler->getGlobalPosition();
		handler->isDragged_ = true;
	}

	// else if (result->is<NEventResult::DropAccepted>()) {
	// 	if (!result->handler->isWidget()) {
	// 		Logger::warn("Non-widget object cannot accept drop!");
	// 	} else {
	// 		NWidget* handler = result->handler->asWidget();
	// 		handler->onDropAccepted(widgetCtx.dragState);
	// 	}
	// 	widgetCtx.dragState.dragged->isDragged = false;
	// 	widgetCtx.dragState.dragged = {};
	// }

	return true;
}

void NRootWidget::draw(Renderer& rdr) const {
	const NCanvas canvas(rdr);
	this->NWidget::draw(canvas);
	if (dragState.dragged) {
		const NCanvas localCanvas = canvas.translated(dragState.dragged->getParent()->getGlobalPosition());
		dragState.dragged->draw(localCanvas);
	}
}

void NRootWidget::update(float dt) {
	NWidget::update(dt);
}