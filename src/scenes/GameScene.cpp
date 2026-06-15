#include "GameScene.h"
#include "../ui/elements/NButton.h"
#include "src/game/Game.h"
#include "src/game/Wands/Wand.h"
#include "src/game/ui/MaterialBar.h"
#include "src/game/ui/NSpell.h"
#include "src/game/ui/NSpellInventory.h"
#include "src/game/ui/ValueBar.h"
#include "src/ui/elements/NRichText.h"

namespace flx::app {
	ui::NRichText* pauseText{};
	ui::MaterialBar* materialBar{};
	ui::ValueBar* expBar{};

	// NSpellSelector* spellSelector = nullptr;

	void GameScene::initUI() {
		createWidget();
		const auto ctx = game.getContext();
		auto& wand = *ctx.gameState.wands.front();

		auto inventory1 = flx::makeUnique(new ui::NSpellInventory({100, 150}, wand.inventory.size()));

		for (const auto i : wand.inventory.indices()) {
			if (!wand.inventory[i]) {
				continue;
			}
			auto spell = flx::makeUnique(new ui::NSpell(wand.inventory[i], {i * 50, i * 50}));
			inventory1->addItem(std::move(spell), static_cast<int>(i));
		}

		inventory1->setOnModify([&](flx::IValView<Shared<game::Spell>> spells) {
			wand.inventory.clear();
			for (auto spell : spells) {
				wand.inventory.emplace_back(spell);
			}
		});

		widget->addToTop(std::move(inventory1));

		auto inventory2 = flx::makeUnique(new ui::NSpellInventory({350, 50}, 5));
		widget->addToTop(std::move(inventory2));

		auto healthBar =
			flx::makeUnique(new ui::ValueBar(
				{windowSize.x - 20, 20},
				{320, 22}, 10, 2.f, ui::ValueBar::HEALTH));
		healthBar->setHealth(10);
		widget->addToTop(std::move(healthBar));

		expBar =
			new ui::ValueBar({windowSize.x - 20, 20 + 22 + 20}, {320, 22},
							 20, 0.f, ui::ValueBar::EXP);
		expBar->setLevel(ctx.gameState.player.level);
		expBar->setHealth(ctx.gameState.player.exp);
		widget->add(flx::makeUnique(expBar));

		materialBar = new ui::MaterialBar({windowSize.x - 20, 20 + 22 + 20 + 22 + 20}, 27.f);
		materialBar->setData(100);
		widget->add(flx::makeUnique(materialBar));

		// spellSelector = new NSpellSelector(windowSize / 2.f, 3, widget);

		pauseText = new ui::NRichText("游戏暂停", 30U);
		pauseText->alignCenter = true;
		pauseText->arrange({0, 0, windowSize.x, windowSize.y / 4.f});
		pauseText->isVisible = false;
		widget->add(flx::makeUnique(pauseText));
	}

	GameScene::GameScene(flx::app::AppContext context)
		: Scene(context, flx::makeContentID<GameScene>()), game(context) {
		this->windowSize = context.windowViewport.canvasSize;
	}

	void GameScene::draw(ui::NRenderBuffer& rdr) {
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

	bool GameScene::handleEvent(const ui::NWindowEvent& event) {
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
} // namespace flx::app