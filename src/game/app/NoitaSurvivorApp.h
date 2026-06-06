#pragma once
#include "src/app/IApp.h"
#include "src/scenes/GameScene.h"
#include "src/scenes/MenuScene.h"

struct NoitaSurvivorApp : flx::app::IApp {
	void setup(flx::app::AppContext context) override {
		context.sceneManager.add(std::make_unique<GameScene>(context));
		context.sceneManager.add(std::make_unique<MenuScene>(context));
		context.sceneManager.setCurrent("menu_scene");
	}
};