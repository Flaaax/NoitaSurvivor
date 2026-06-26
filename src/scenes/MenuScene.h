#pragma once

#include "../app/Scene.h"

namespace flx::app {
	class MenuScene : public Scene {
	public:
		explicit MenuScene(AppCtx context);

		void draw(ui::RenderBuffer& rdr) override;
		void update(float dt) override;
	};
} // namespace flx::app