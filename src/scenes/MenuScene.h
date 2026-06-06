#pragma once

#include "../app/Scene.h"

class MenuScene : public flx::app::Scene {
public:
	explicit MenuScene(flx::app::AppContext context);

	void draw(NRenderBuffer& rdr) override;
	void update(float dt) override;
};