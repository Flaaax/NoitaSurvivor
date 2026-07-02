#pragma once
#include "src/app/IApp.h"
#include "src/scenes/GameScene.h"
#include "src/scenes/MenuScene.h"
#include "src/scenes/SettingsScene.h"

namespace flx::app {
	struct NoitaSurvivorApp : IApp {
		void setup(AppCtx ctx) override {
			auto gameScene = ctx.sceneManager.add(makeSUnique<GameScene>(ctx));
			auto menuScene = ctx.sceneManager.add(makeSUnique<MenuScene>(ctx));
			const auto settingsScene = ctx.sceneManager.add(makeSUnique<SettingsScene>(ctx));
			ctx.sceneManager.addCommand({.target = "menu_scene", .cmd = SceneCmd::Enter});
			ctx.runtime.showDebugFPS = true;
			ctx.runtime.imguiDisplay = true;

			gameScene->requestTogglePause = [ctx](bool pause) {
				if (pause) {
					ctx.sceneManager.addCommand({.target = "settings_scene", .cmd = SceneCmd::Enter});
				} else {
					ctx.sceneManager.addCommand({.target = "settings_scene", .cmd = SceneCmd::Exit});
				}
			};

			settingsScene->requestResume = [ctx, gameScene] {
				gameScene->setPause(false);
				ctx.sceneManager.addCommand({.target = "settings_scene", .cmd = SceneCmd::Exit});
			};
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
