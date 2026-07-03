#include "GameScene.h"
#include "../ui/elements/Button.h"
#include "src/app/global/DebugVariables.h"
#include "src/game/Game.h"
#include "src/game/Wands/Wand.h"
#include "src/game/ui/MaterialBar.h"
#include "src/game/ui/SpellInventory.h"
#include "src/game/ui/ValueBar.h"
#include "src/game/ui/WandEditor.h"
#include "src/ui/context/WindowEvent.h"
#include "src/ui/elements/RichText.h"
#include "src/ui/render/RenderBuffer.h"

#include <imgui.h>

namespace flx::app {
	// NSpellSelector* spellSelector = nullptr;

	void GameScene::initUI() {
		createWidget();
		const auto gctx = game.getContext();
		// auto& wand = *ctx.gameState.wands.front();
		// const auto wand = ctx.gameState.wandManager.getWand(0);

		{
			auto editor = makeSUnique<ui::WandEditor>();
			this->editor = editor;
			editor->setPosition({100, 150});
			editor->setWands(gctx.gameState.wandManager);
			widget->addToTop(std::move(editor));
		}

		{
			auto inventory = makeSUnique<ui::SpellInventory>(vec2{editor->getInventoryX(), 75}, 5);
			this->inventory = inventory;
			// inventoryU->setPosition({editor->getInventoryX(), 50});
			widget->addToTop(inventory | move)->ref();
		}

		const auto windowSize = ctx.window.getView().canvasSize;

		auto healthBar =
			makeUnique(new ui::ValueBar(
				{windowSize.x - 20, 20},
				{320, 22}, 10, 2.f, ui::ValueBar::HEALTH));
		healthBar->setHealth(10);
		widget->addToTop(std::move(healthBar));

		expBar =
			new ui::ValueBar({windowSize.x - 20, 20 + 22 + 20}, {320, 22},
							 20, 0.f, ui::ValueBar::EXP);
		expBar->setLevel(gctx.gameState.player.level);
		expBar->setHealth(gctx.gameState.player.exp);
		widget->add(makeUnique(expBar));

		materialBar = new ui::MaterialBar({windowSize.x - 20, 20 + 22 + 20 + 22 + 20}, 27.f);
		materialBar->setData(100);
		widget->add(makeUnique(materialBar));

		// spellSelector = new NSpellSelector(windowSize / 2.f, 3, widget);

		pauseText = new ui::RichText("游戏暂停", 30U);
		pauseText->alignCenter = true;
		pauseText->arrange({0, 0, windowSize.x, windowSize.y / 3.f});
		pauseText->isVisible = false;
		widget->add(makeUnique(pauseText));
	}

	GameScene::GameScene(app::AppCtx ctx)
		: Scene(ctx, makeContentID<GameScene>()),
		  game(ctx) {
	}

	void GameScene::draw(ui::RenderBuffer& buffer) {
		game.draw(buffer);

		// pauseText->isVisible = game.isPaused();
		if (game.isPaused()) {
			sf::RectangleShape overlay{ctx.window.getView().canvasSize};
			overlay.setFillColor({0, 0, 0, 120});
			buffer.drawCanvas(overlay);
			Scene::draw(buffer);
			buffer.drawUI(overlay);
		} else {
			Scene::draw(buffer);
		}
	}

	void GameScene::update(float dt) {
		// if (spellSelector && !spellSelector->getIsRunning()) {
		//	m_widget->destroy(spellSelector);
		//	spellSelector = nullptr;
		// }

		game.update(dt);
		const auto ctx = game.getContext();
		const auto& p = ctx.gameState.player;
		materialBar->setData(p.material);
		expBar->setValue(p.exp, p.maxExp, p.level);

		Scene::update(dt);
	}

	bool GameScene::handleEvent(const ui::WindowEvent& event) {
		if (Scene::handleEvent(event)) {
			return true;
		}

		if (const auto e = event.rawEvent.getIf<sf::Event::KeyPressed>()) {
			if (e->code == sf::Keyboard::Key::Space) {
				game.setPaused(!game.isPaused());
				requestTogglePause(game.isPaused());
				return true;
			}
			if (e->code == sf::Keyboard::Key::F) {
				editor->isVisible = !editor->isVisible;
				return true;
			}
		}

		game.handleEvent(event.rawEvent);

		return false;
	}

	void GameScene::enter() {
		if (init) {
			return;
		}
		game.init();
		initUI();
		init = true;
		logger.info("GameScene initialized");
	}

	namespace {
		bool drawStringCombo(const char* label, Span<const char*> items, int& current_index) {
			if (items.empty())
				return false;

			const char* preview = items[current_index];
			bool changed = false;

			if (ImGui::BeginCombo(label, preview)) {
				for (const int i : items.indices<int>()) {
					const bool selected = (current_index == i);

					if (ImGui::Selectable(items[i], selected)) {
						current_index = i;
						changed = true;
					}

					if (selected) {
						ImGui::SetItemDefaultFocus();
					}
				}

				ImGui::EndCombo();
			}
			return changed;
		}
	} // namespace

	void GameScene::makeImGuiContent() {
		if (!ImGui::CollapsingHeader("游戏内容", ImGuiTreeNodeFlags_DefaultOpen)) {
			return;
		}

		if (ImGui::Button("清除实体")) {
			static bool& shouldClear = DebugVariables::emplace<bool>("shouldClearEntities", true);
			shouldClear = true;
		}

		static bool& enableEnemySpawn = DebugVariables::emplace<bool>("enableEnemySpawn", true);
		if (ImGui::Button(!enableEnemySpawn ? "启用怪物生成" : "禁用怪物生成")) {
			enableEnemySpawn = !enableEnemySpawn;
		}
		if (enableEnemySpawn) {
			static float& enemySpawnFreq = DebugVariables::emplace<float>("enemySpawnFreq", 1.f);
			ImGui::SliderFloat("怪物生成速率", &enemySpawnFreq, 0.5f, 10.f);
		}

		static Vector<const char*> trackers = {
			"none",
			"circle",
			"seek",
			"weakSeek",
			"leadSeek",
			"lateral",
			"navigation",
		};

		static int& selectedTracker = DebugVariables::emplace("tracker", 1);

		if (drawStringCombo("跟踪算法", trackers, selectedTracker)) {
			// Logger::info("选择了 {}", trackers[selected]);
		}

		static float& cameraScale = DebugVariables::emplace<float>("cameraScale", 1.f);

		ImGui::SliderFloat("缩放", &cameraScale, 0.5f, 2.f, "%.3f", ImGuiSliderFlags_NoInput);
		ImGui::SameLine();
		if (ImGui::Button("重置")) {
			cameraScale = 1.f;
		}

		game.getContext().gameState.camera.scale = {cameraScale, cameraScale};
	}

	void GameScene::setPause(bool pause) {
		game.setPaused(pause);
	}
} // namespace flx::app