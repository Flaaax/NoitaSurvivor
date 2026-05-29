#pragma once
#include "NWidget.h"
#include "src/ui/context/NStyle.h"

class NPanel;
class NTooltip;
class Renderer;

class NRootWidget : public NWidget {
private:
	NDragState dragState;
	NHoverState hoverState;
	NStyle style;
	n_unique<NPanel> tooltip;
	nvec2 mousePosition{};
	bool handleDragEvent(const NEventCtx& event);
	static bool shouldHandleEvent(const NEventCtx& ctx);
	void updateTooltipContent();
	void updateHover(float dt);

public:
	explicit NRootWidget(nrect geometry = {0.f, 0.f, 100.f, 100.f}, bool updateEnabled_ = true);
	// Handle event as the root widget
	bool handleEvent(const NEventCtx& ctx);
	void draw(Renderer& rdr) const;
	void update(float dt) override;
	//void setStyle(NStyle style);
};