#include "RootWidget.h"

#include "../elements/Tooltip.h"
#include "src/ui/elements/Panel.h"
#include "src/ui/global/Global.h"
#include "src/ui/render/Painter.h"

namespace flx::ui {
	bool RootWidget::handleDragEvent(const WindowEvent& event) {
		if (event.rawEvent.is<sf::Event::MouseMoved>()) {
			const vec2 newGlobalPosition = event.input.mouseRender - dragState.offset;
			const vec2 newLocalPosition = dragState.dragged->getParentLocalPosition(newGlobalPosition);
			dragState.dragged->setPosition(newLocalPosition);
		}

		const DropQuery query{
			.state = dragState,
			.globalHitbox = dragState.dragged->getGlobalHitbox(),
		};
		DropCollector collector;
		onDropQuery(query, collector);
		Object* candidate{};
		if (!collector.candidates.empty()) {
			candidate = collector.candidates.best({}, &DropCandidate::score).target;
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

	bool RootWidget::handleEvent(const WindowEvent& ctx) {
		if (!shouldHandleEvent(ctx)) {
			return false;
		}
		const RootCtx widgetCtx{
			.dragState = dragState,
		};
		const UIEvent event{
			.windowEvent = ctx,
			.rootCtx = widgetCtx,
			.localCtx = LocalEventCtx{
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

		const auto result = this->Widget::handleEvent(event);

		if (result) {
			if (result->is<EventResult::DragIntent>()) {
				Object* handler = result->handler;
				dragState.dragged = handler;
				dragState.offset = ctx.input.mouseRender - handler->getGlobalPosition();
				handler->isDragged_ = true;
				return true;
			}

			if (result->is<EventResult::HoverIntent>()) {
				Object* handler = result->handler;
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

	bool RootWidget::shouldHandleEvent(const WindowEvent& ctx) {
		const auto& raw = ctx.rawEvent;
		return raw.is<sf::Event::MouseButtonPressed>() ||
			   raw.is<sf::Event::MouseButtonReleased>() ||
			   raw.is<sf::Event::MouseMoved>() ||
			   raw.is<sf::Event::FocusLost>() ||
			   raw.is<sf::Event::FocusGained>();
	}

	void RootWidget::updateTooltipContent() {
		tooltip->setSize({hoverState.target->tooltipSpec.width, 0});
		tooltip->setLayout(hoverState.target->tooltipSpec.builder(style, hoverState.target));
		hoverState.tooltipDirty = false;
	}

	void RootWidget::updateHover(float dt) {
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

		if (hoverState.hoveredTime >= hoverState.hoverIntentDelay && hoverState.target->tooltipSpec.builder) {
			if (!tooltip) {
				tooltip = std::make_unique<Panel>();
				tooltip->parent = this;
				tooltip->backgroundColor = {160, 160, 160};
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

	RootWidget::RootWidget(Viewport viewport, rect geometry, bool updateEnabled_)
		: Widget(geometry, updateEnabled_),
		  style({.font = Global::getDefaultFont()}),
		  viewport(viewport) {
		hoverState.hoverIntentDelay = 0.22f;
		isRoot = true;
	}

	void RootWidget::draw(RenderBuffer& rdr) const {
		const UIPainter canvas(rdr, getPosition());
		this->Widget::draw(canvas);
		if (dragState.dragged) {
			dragState.dragged->draw(canvas.translated(dragState.dragged->getGlobalPosition()));

		} else if (tooltip && tooltip->isVisible) {
			tooltip->draw(canvas.translated(tooltip->getPosition()));
			// Logger::info("Should draw tooltip");
		}
	}

	void RootWidget::update(float dt) {
		updateHover(dt);
		Widget::update(dt);
	}

	const Viewport& RootWidget::getViewport() const {
		return viewport;
	}

	void RootWidget::onWindowResized(const WindowView& view) {
		this->viewport = view.viewport;
		Widget::refresh();
		if (tooltip) {
			tooltip->refresh();
		}
	}

	// void NRootWidget::setStyle(NStyle style) {
	// 	this->style = style;
	// }
} // namespace flx::ui