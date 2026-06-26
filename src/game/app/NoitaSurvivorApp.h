#pragma once
#include "src/app/IApp.h"
#include "src/scenes/GameScene.h"
#include "src/scenes/MenuScene.h"

namespace flx::app {
	struct NoitaSurvivorApp : IApp {
		void setup(AppCtx ctx) override {
			ctx.sceneManager.add(std::make_unique<GameScene>(ctx));
			ctx.sceneManager.add(std::make_unique<MenuScene>(ctx));
			ctx.sceneManager.addCommand({.target = "menu_scene", .cmd = SceneCmd::Enter});
			ctx.runtime.showDebugFPS = true;
			ctx.runtime.imguiDisplay = true;
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
