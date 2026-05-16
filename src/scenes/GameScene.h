#pragma once
#ifndef GAMESCENE_H
#define GAMESCENE_H

#include "src/ui/NScene.h"
#include "src/utils/Singleton.h"

class Game;

class GameScene :public NScene {
	N_REG_SCENE(GameScene);
	N_DECL_INITABLE;
private:
	Game& game;
	logptr logger;

	void onInit();
	void initUI();
public:
	GameScene();
	~GameScene();

	void draw(Renderer& renderer) override;
	void update(float deltaTime) override;
	void handleEvent(const sf::Event& event)override;
	void enter()override;
	void exit()override;
};


#endif