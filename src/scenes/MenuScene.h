#pragma once

#include "../ui/NScene.h"

class MenuScene :public NScene {
	N_REG_SCENE(MenuScene)
public:
	MenuScene();

	void draw(Renderer& renderer) override;
	void update(float deltaTime) override;
};