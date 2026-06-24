#pragma once

#include "../app/Scene.h"

namespace flx::app {
	class MenuScene : public flx::app::Scene {
	public:
		explicit MenuScene(flx::app::AppCtx context);

		void draw(ui::RenderBuffer& rdr) override;
		void update(float dt) override;
	};
} // namespace flx::app