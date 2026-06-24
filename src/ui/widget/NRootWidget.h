#pragma once
#include "NWidget.h"
#include "src/ui/context/NStyle.h"
#include "src/ui/render/NViewport.h"

namespace flx::ui {
	class NPanel;
	class NTooltip;
	class RenderBuffer;

	class RootWidget : public Widget {
	private:
		NDragState dragState;
		NHoverState hoverState;
		NStyle style;
		Unique<NPanel> tooltip;
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

		void onWindowResized(const NWindowView& view);

		// void setStyle(NStyle style);
	};
} // namespace flx::ui