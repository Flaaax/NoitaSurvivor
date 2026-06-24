#include "GameScene.h"
#include "../ui/elements/NButton.h"
#include "src/app/global/DebugVariables.h"
#include "src/game/Game.h"
#include "src/game/Wands/Wand.h"
#include "src/game/ui/MaterialBar.h"
#include "src/game/ui/NSpell.h"
#include "src/game/ui/NSpellInventory.h"
#include "src/game/ui/ValueBar.h"
#include "src/ui/elements/NRichText.h"

#include <imgui.h>

namespace flx::app {
	ui::NRichText* pauseText{};
	ui::MaterialBar* materialBar{};
	ui::ValueBar* expBar{};

	// NSpellSelector* spellSelector = nullptr;

	void GameScene::initUI() {
		createWidget();
		const auto ctx = game.getContext();
		// auto& wand = *ctx.gameState.wands.front();
		const auto wand = ctx.gameState.wandManager.getWand(0);

		auto inventory1 = makeUnique(new ui::NSpellInventory({100, 150}, wand->inventory.size()));

		for (const auto i : wand->inventory.indices()) {
			if (!wand->inventory[i]) {
				continue;
			}
			auto spell = makeUnique(new ui::NSpell(wand->inventory[i], {i * 50, i * 50}));
			inventory1->addItem(std::move(spell), static_cast<int>(i));
		}

		inventory1->setOnModify([=](viewable::Val<Shared<game::Spell>> spells) {
			wand->inventory.clear();
			for (auto spell : spells) {
				wand->inventory.emplace_back(spell);
			}
		});

		widget->addToTop(std::move(inventory1));

		auto inventory2 = makeUnique(new ui::NSpellInventory({350, 50}, 5));
		widget->addToTop(std::move(inventory2));

		auto healthBar =
			makeUnique(new ui::ValueBar(
				{windowSize.x - 20, 20},
				{320, 22}, 10, 2.f, ui::ValueBar::HEALTH));
		healthBar->setHealth(10);
		widget->addToTop(std::move(healthBar));

		expBar =
			new ui::ValueBar({windowSize.x - 20, 20 + 22 + 20}, {320, 22},
							 20, 0.f, ui::ValueBar::EXP);
		expBar->setLevel(ctx.gameState.player.level);
		expBar->setHealth(ctx.gameState.player.exp);
		widget->add(makeUnique(expBar));

		materialBar = new ui::MaterialBar({windowSize.x - 20, 20 + 22 + 20 + 22 + 20}, 27.f);
		materialBar->setData(100);
		widget->add(makeUnique(materialBar));

		// spellSelector = new NSpellSelector(windowSize / 2.f, 3, widget);

		pauseText = new ui::NRichText("游戏暂停", 30U);
		pauseText->alignCenter = true;
		pauseText->arrange({0, 0, windowSize.x, windowSize.y / 4.f});
		pauseText->isVisible = false;
		widget->add(makeUnique(pauseText));
	}

	GameScene::GameScene(app::AppCtx context)
		: Scene(context, makeContentID<GameScene>()), game(context) {
		this->windowSize = context.windowViewport.canvasSize;
	}

	void GameScene::draw(ui::RenderBuffer& rdr) {
		game.draw(rdr);

		pauseText->isVisible = game.isPaused();
		Scene::draw(rdr);
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
		bool drawStringCombo(const char* label, const flx::Vector<std::string_view>& items, int& current_index) {
			if (items.empty())
				return false;

			const char* preview = items[current_index].data();
			bool changed = false;

			if (ImGui::BeginCombo(label, preview)) {
				for (const int i : items.indices<int>()) {
					const bool selected = (current_index == i);

					if (ImGui::Selectable(items[i].data(), selected)) {
						current_index = i;
						changed = true;
					}

					if (selected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}
			return changed;
		}
	} // namespace

	void GameScene::makeImGuiContent() {
		ImGui::SeparatorText("游戏内容");

		if (ImGui::Button("清除实体")) {
			static bool& shouldClear = DebugVariables::try_emplace<bool>("shouldClearEntities", true);
			shouldClear = true;
		}

		static bool& enableEnemySpawn = DebugVariables::try_emplace<bool>("enableEnemySpawn", true);
		if (ImGui::Button(!enableEnemySpawn ? "启用怪物生成" : "禁用怪物生成")) {
			enableEnemySpawn = !enableEnemySpawn;
		}
		if (enableEnemySpawn) {
			static float& enemySpawnFreq = DebugVariables::try_emplace<float>("enemySpawnFreq", 1.f);
			ImGui::SliderFloat("怪物生成速率", &enemySpawnFreq, 0.5f, 10.f);
		}

		static Vector<std::string_view> trackers = {
			"none",
			"circle",
			"seek",
			"weakSeek",
			"leadSeek",
			"lateral",
			"navigation",
		};

		static int& selectedTracker = DebugVariables::try_emplace("tracker", 1);

		if (drawStringCombo("跟踪算法", trackers, selectedTracker)) {
			// Logger::info("选择了 {}", trackers[selected]);
		}
	}
} // namespace flx::app