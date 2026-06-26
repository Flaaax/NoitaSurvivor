#pragma once
#include "Widget.h"
#include "src/ui/context/Style.h"
#include "src/ui/render/Viewport.h"

namespace flx::ui {
	class Panel;
	class Tooltip;
	class RenderBuffer;

	class RootWidget : public Widget {
	private:
		DragState dragState;
		HoverState hoverState;
		Style style;
		Unique<Panel> tooltip;
		vec2 mousePosition{};
		Viewport viewport;

		bool handleDragEvent(const WindowEvent& event);
		static bool shouldHandleEvent(const WindowEvent& ctx);
		void updateTooltipContent();
		void updateHover(float dt);

	public:
		explicit RootWidget(Viewport viewport, rect geometry = {0.f, 0.f, 100.f, 100.f}, bool updateEnabled_ = true);
		// Handle event as the root widget
		bool handleEvent(const WindowEvent& ctx);
		void draw(RenderBuffer& rdr) const;
		void update(float dt) override;
		const Viewport& getViewport() const;

		void onWindowResized(const WindowView& view);

		// void setStyle(NStyle style);
	};
} // namespace flx::ui