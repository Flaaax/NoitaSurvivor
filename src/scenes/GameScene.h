#pragma once
#ifndef GAMESCENE_H
#define GAMESCENE_H

#include "../app/Scene.h"
#include "src/game/Game.h"

namespace flx::app {
	class GameScene : public flx::app::Scene {
	private:
		game::Game game;
		bool init{};
		vec2 windowSize{};

		void initUI();

	public:
		explicit GameScene(flx::app::AppContext context);

		void draw(ui::NRenderBuffer& rdr) override;
		void update(float dt) override;
		bool handleEvent(const ui::NWindowEvent& event) override;
		void enter() override;
	};
} // namespace flx::app

#endif