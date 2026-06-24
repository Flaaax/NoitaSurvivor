#pragma once
#ifndef GAMESCENE_H
#define GAMESCENE_H

#include "../app/Scene.h"
#include "src/game/Game.h"

namespace flx::app {
	class GameScene : public Scene {
	private:
		game::Game game;
		bool init{};
		vec2 windowSize{};

		void initUI();

	public:
		explicit GameScene(AppCtx context);

		void draw(ui::RenderBuffer& rdr) override;
		void update(float dt) override;
		bool handleEvent(const ui::WindowEvent& event) override;
		void enter() override;
		void makeImGuiContent() override;
	};
} // namespace flx::app

#endif