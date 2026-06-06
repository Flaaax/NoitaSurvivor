#pragma once
#ifndef GAMESCENE_H
#define GAMESCENE_H

#include "../app/Scene.h"
#include "src/game/Game.h"

class GameScene : public flx::app::Scene {
private:
	Game game;
	bool init{};
	nvec2 windowSize{};

	void initUI();

public:
	explicit GameScene(flx::app::AppContext context);

	void draw(NRenderBuffer& rdr) override;
	void update(float dt) override;
	bool handleEvent(const NWindowEvent& event) override;
	void enter() override;
};

#endif