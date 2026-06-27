#include "GameScene.h"
#include "../ui/elements/Button.h"
#include "src/app/global/DebugVariables.h"
#include "src/game/Game.h"
#include "src/game/Wands/Wand.h"
#include "src/game/ui/MaterialBar.h"
#include "src/game/ui/Spell.h"
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

		auto editor = std::make_unique<ui::WandEditor>();
		editor->setPosition({100, 150});
		editor->setWands(gctx.gameState.wandManager);
		// auto inventory1 = makeUnique(new ui::SpellInventory({100, 150}, wand->inventory.size()));
		//
		// for (const auto i : wand->inventory.indices()) {
		// 	if (!wand->inventory[i]) {
		// 		continue;
		// 	}
		// 	auto spell = makeUnique(new ui::Spell(wand->inventory[i], {i * 50, i * 50}));
		// 	inventory1->addItem(std::move(spell), static_cast<int>(i));
		// }
		//
		// inventory1->setOnModify([=](viewable::Val<Shared<game::Spell>> spells) {
		// 	wand->inventory.clear();
		// 	for (auto spell : spells) {
		// 		wand->inventory.emplace_back(spell);
		// 	}
		// });

		widget->addToTop(std::move(editor));

		auto inventory2 = makeUnique(new ui::SpellInventory({100, 50}, 5));
		widget->addToTop(std::move(inventory2));

		const auto windowSize = ctx.windowView.canvasSize;

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
			sf::RectangleShape overlay{ctx.windowView.canvasSize};
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
				requestPause();
				return true;
			}
		}

		game.handleEvent(event.rawEvent);

		return true;
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
		ImGui::SeparatorText("游戏内容");

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
	}

	void GameScene::onWindowResized(const ui::WindowView& view) {
		Scene::onWindowResized(view);
		// pauseText->arrange({0, 0, windowSize.x, windowSize.y / 4.f});
	}

	void GameScene::setPause(bool pause) {
		game.setPaused(pause);
	}
} // namespace flx::app