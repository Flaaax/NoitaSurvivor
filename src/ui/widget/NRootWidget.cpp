#include "NRootWidget.h"

#include "src/global/AssetManager.h"
#include "src/ui/NTooltip.h"
#include "src/ui/render/NCanvas.h"

bool NRootWidget::handleDragEvent(const NEventCtx& event) {
	if (event.rawEvent.is<sf::Event::MouseMoved>()) {
		const nvec2 newGlobalPosition = event.input.mouseRender - dragState.offset;
		const nvec2 newLocalPosition = dragState.dragged->getParentLocalPosition(newGlobalPosition);
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
		candidate = collector.candidates.best({}, &NDropCandidate::score).target;
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
	if (!shouldHandleEvent(ctx)) {
		return false;
	}
	const NRootCtx widgetCtx{
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

	if (hoverState.target && tooltip) {
		tooltip->setPosition(ctx.input.mouseRender);
	}

	if (ctx.rawEvent.is<sf::Event::MouseMoved>()) {
		hoverState.mouseMovedInFrame = true;
		mousePosition = ctx.input.mouseRender;
	}

	const auto result = this->NWidget::handleEvent(event);

	if (result) {
		if (result->is<NEventResult::DragIntent>()) {
			NObject* handler = result->handler;
			dragState.dragged = handler;
			dragState.offset = ctx.input.mouseRender - handler->getGlobalPosition();
			handler->isDragged_ = true;
			return true;
		}

		if (result->is<NEventResult::HoverIntent>()) {
			NObject* handler = result->handler;
			if (handler != hoverState.target) {
				hoverState.hoveredTime = 0.f;
				hoverState.tooltipDirty = true;
				hoverState.hoveredTime = hoverState.hoverIntentDelay; // Make continuous hover
			}
			hoverState.target = handler;
			hoverState.hasTargetInFrame = true;
			return true;
		}
	}

	return false;
}

bool NRootWidget::shouldHandleEvent(const NEventCtx& ctx) {
	const auto& raw = ctx.rawEvent;
	return raw.is<sf::Event::MouseButtonPressed>() ||
		   raw.is<sf::Event::MouseButtonReleased>() ||
		   raw.is<sf::Event::MouseMoved>() ||
		   raw.is<sf::Event::FocusLost>() ||
		   raw.is<sf::Event::FocusGained>();
}

void NRootWidget::updateTooltipContent() {
	tooltip->setContent(*hoverState.target->tooltipSpec, style.tooltipStyle);
	hoverState.tooltipDirty = false;
}

void NRootWidget::updateHover(float dt) {
	if (!hoverState.hasTargetInFrame && hoverState.mouseMovedInFrame) {
		hoverState.target = {};
		if (tooltip) {
			tooltip->isVisible = false;
		}
		hoverState.tooltipDirty = true;
	}

	hoverState.mouseMovedInFrame = false;
	hoverState.hasTargetInFrame = false;

	if (!hoverState.target) {
		return;
	}

	if (hoverState.hoveredTime < hoverState.hoverIntentDelay) {
		hoverState.hoveredTime += dt;
	}

	if (hoverState.hoveredTime >= hoverState.hoverIntentDelay && hoverState.target->tooltipSpec) {
		if (!tooltip) {
			tooltip = Util::makeUnique(new NTooltip());
			updateTooltipContent();
		} else if (hoverState.tooltipDirty) {
			updateTooltipContent();
		}
		tooltip->isVisible = true;
		if (tooltip) {
			tooltip->setPosition(mousePosition);
		}
	}
}

NRootWidget::NRootWidget(nrect geometry, bool updateEnabled_) : NWidget(geometry, updateEnabled_) {
	hoverState.hoverIntentDelay = 0.22f;
	style.font = &AssetMgr::getDefaultFont();
	style.tooltipStyle.font = style.font;
}

void NRootWidget::draw(Renderer& rdr) const {
	const NCanvas canvas(rdr, getPosition());
	this->NWidget::draw(canvas);
	if (dragState.dragged) {
		dragState.dragged->draw(canvas.translated(dragState.dragged->getGlobalPosition()));

	} else if (tooltip && tooltip->isVisible) {
		tooltip->draw(canvas.translated(tooltip->getPosition()));
		// Logger::info("Should draw tooltip");
	}
}

void NRootWidget::update(float dt) {
	updateHover(dt);
	NWidget::update(dt);
}