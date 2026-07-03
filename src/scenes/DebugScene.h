#pragma once
#include "src/app/Scene.h"

namespace flx::app {
	class DebugScene : public Scene {
	private:
	public:
		explicit DebugScene(AppCtx ctx);
		void makeImGuiContent() override;
		bool handleEvent(const ui::WindowEvent& event) override;
	};
} // namespace flx::app