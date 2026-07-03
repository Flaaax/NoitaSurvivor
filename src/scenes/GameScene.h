#pragma once

#include "../app/Scene.h"
#include "src/game/Game.h"
#include "src/utils/Fon/Fon.h"

namespace flx::ui {
	class WandEditor;
	class SpellInventory;
	class ValueBar;
	class MaterialBar;
	class RichText;
}

namespace flx::app {
	class GameScene : public Scene {
	private:
		game::Game game;
		bool init{};
		Fon gameCfg;
		//vec2 windowSize{};

		ui::RichText* pauseText{};
		ui::MaterialBar* materialBar{};
		ui::ValueBar* expBar{};
		ui::Ref<ui::SpellInventory> inventory{};
		ui::Ref<ui::WandEditor> editor{};

		void initUI();

	public:
		std::function<void(bool)> requestTogglePause{};

		explicit GameScene(AppCtx ctx);
		~GameScene() override;

		void draw(ui::RenderBuffer& buffer) override;
		void update(float dt) override;
		bool handleEvent(const ui::WindowEvent& event) override;
		void enter() override;
		void makeImGuiContent() override;

		void setPause(bool pause);

	};
} // namespace flx::app