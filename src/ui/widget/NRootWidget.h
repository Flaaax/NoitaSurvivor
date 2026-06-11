#pragma once
#include "NWidget.h"
#include "src/ui/context/NStyle.h"

struct NViewport;
class NPanel;
class NTooltip;
class NRenderBuffer;

class NRootWidget : public NWidget {
private:
	NDragState dragState;
	NHoverState hoverState;
	NStyle style;
	n_unique<NPanel> tooltip;
	nvec2 mousePosition{};
	NViewport viewport;

	bool handleDragEvent(const NWindowEvent& event);
	static bool shouldHandleEvent(const NWindowEvent& ctx);
	void updateTooltipContent();
	void updateHover(float dt);

public:
	explicit NRootWidget(NViewport viewport, nrect geometry = {0.f, 0.f, 100.f, 100.f}, bool updateEnabled_ = true);
	// Handle event as the root widget
	bool handleEvent(const NWindowEvent& ctx);
	void draw(NRenderBuffer& rdr) const;
	void update(float dt) override;
	const NViewport& getViewport() const;

	void onWindowResized(const NWindowView& view);

	// void setStyle(NStyle style);
};