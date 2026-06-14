#pragma once
#include "src/app/IApp.h"
#include "src/scenes/GameScene.h"
#include "src/scenes/MenuScene.h"

namespace flx::app {
	struct NoitaSurvivorApp : IApp {
		void setup(AppContext context) override {
			context.sceneManager.add(std::make_unique<GameScene>(context));
			context.sceneManager.add(std::make_unique<MenuScene>(context));
			context.sceneManager.setCurrent("menu_scene");
		}
	};
} // namespace flx::game
