#pragma once
#include "NWidget.h"
#include "src/ui/context/NStyle.h"
#include "src/ui/render/NViewport.h"

namespace flx::ui {
	class NPanel;
	class NTooltip;
	class RenderBuffer;

	class NRootWidget : public NWidget {
	private:
		NDragState dragState;
		NHoverState hoverState;
		NStyle style;
		Unique<NPanel> tooltip;
		vec2 mousePosition{};
		NViewport viewport;

		bool handleDragEvent(const WindowEvent& event);
		static bool shouldHandleEvent(const WindowEvent& ctx);
		void updateTooltipContent();
		void updateHover(float dt);

	public:
		explicit NRootWidget(NViewport viewport, rect geometry = {0.f, 0.f, 100.f, 100.f}, bool updateEnabled_ = true);
		// Handle event as the root widget
		bool handleEvent(const WindowEvent& ctx);
		void draw(RenderBuffer& rdr) const;
		void update(float dt) override;
		const NViewport& getViewport() const;

		void onWindowResized(const NWindowView& view);

		// void setStyle(NStyle style);
	};
} // namespace flx::ui