#pragma once
#include "src/app/IApp.h"
#include "src/scenes/GameScene.h"
#include "src/scenes/MenuScene.h"

namespace flx::app {
	struct NoitaSurvivorApp : IApp {
		void setup(AppCtx context) override {
			context.sceneManager.add(std::make_unique<GameScene>(context));
			context.sceneManager.add(std::make_unique<MenuScene>(context));
			context.sceneManager.setCurrent("menu_scene");
			context.runtime.showDebugFPS = true;
			context.runtime.imguiDisplay = true;
		}

		AppInfo getInfo() override {
			return {
				.name = "NoitaSurvivor",
				.defaultWindowSize = {1920, 1080},
				.defaultFont = "fonts/msyh.ttc",
				.imguiEnabled = true,
			};
		}
	};
} // namespace flx::app
