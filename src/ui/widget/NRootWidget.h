#pragma once
#include "NWidget.h"

class Renderer;

class NRootWidget : public NWidget {
private:
	NDragState dragState;
	bool handleDragEvent(const NEventCtx& event);

public:
	// Handle event as the root widget
	bool handleEvent(const NEventCtx& ctx);
	void draw(Renderer& rdr) const;
	void update(float dt) override;
};