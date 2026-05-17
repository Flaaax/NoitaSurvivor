#pragma once
#ifndef GAMESCENE_H
#define GAMESCENE_H

#include "src/ui/NScene.h"
#include "src/utils/Singleton.h"

class Game;

class GameScene : public NScene {
private:
	Game& game;
	logptr logger;
	bool init = false;

	void initUI();

public:
	GameScene();

	void draw(Renderer& rdr) override;
	void update(float dt) override;
	bool handleEvent(const NEventCtx& event) override;
	void enter() override;
	std::string_view getName() const override;
};

#endif