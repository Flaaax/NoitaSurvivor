#pragma once
#include "src/app/Scene.h"

namespace flx::app {
	class SettingsScene : public Scene {
	public:
		std::function<void()> requestResume{};

		explicit SettingsScene(AppCtx ctx);
		void setVisible(bool visible) const;

		// void enter() override;
		// void exit() override;
	};
} // namespace flx::app