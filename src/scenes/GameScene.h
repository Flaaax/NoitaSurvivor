#pragma once
#ifndef GAMESCENE_H
#define GAMESCENE_H

#include "../app/Scene.h"

class Game;

class GameScene : public flx::app::Scene {
private:
	Game& game;
	bool init{};
	nvec2 windowSize{};

	void initUI();

public:
	GameScene(nvec2 windowSize);

	void draw(NRenderBuffer& rdr) override;
	void update(float dt) override;
	bool handleEvent(const NWindowEvent& event) override;
	void enter() override;
};

#endif