#include "GameScene.h"
#include "../ui/NButton.h"
#include "src/game/Game.h"
#include "src/game/Wands/Wand.h"
#include "src/game/ui/MaterialBar.h"
#include "src/game/ui/NSpell.h"
#include "src/game/ui/NSpellInventory.h"
#include "src/game/ui/ValueBar.h"
#include "src/ui/NText.h"
#include <src/utils/Logger.h>

NText* pauseText{};
MaterialBar* materialBar{};
ValueBar* expBar{};

// NSpellSelector* spellSelector = nullptr;

GameScene::GameScene()
	: game(Game::inst()), logger(LoggerFactory::createAsync("GameScene")) {
	Logger::info("GameScene created");
}

void GameScene::draw(Renderer& rdr) {
	game.draw(rdr);

	pauseText->isVisible = game.isPaused();
	NScene::draw(rdr);
}

void GameScene::update(float dt) {
	// if (spellSelector && !spellSelector->getIsRunning()) {
	//	m_widget->destroy(spellSelector);
	//	spellSelector = nullptr;
	// }

	game.update(dt);
	const auto& p = game.state.player;
	materialBar->setData(p.material);
	expBar->setValue(p.exp, p.maxExp, p.level);

	NScene::update(dt);
}

bool GameScene::handleEvent(const NEventCtx& event) {
	if (NScene::handleEvent(event)) {
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
	Logger::info("GameScene initialized");
}

std::string_view GameScene::getName() const {
	return NObject::makeTypeID<GameScene>();
}

void GameScene::initUI() {
	createWidget();
	auto& wand = *game.state.wands.front();

	auto inventory1 = Util::makeUnique(new NSpellInventory({100, 150}, wand.inventory.size()));

	for (const auto i : wand.inventory.indices()) {
		if (!wand.inventory[i]) {
			continue;
		}
		auto spell = Util::makeUnique(new NSpell(wand.inventory[i], {i * 50, i * 50}));
		inventory1->addItem(std::move(spell), static_cast<int>(i));
	}

	inventory1->setOnModify([&](Util::ValEnumerableView<n_shared<Spell>> spells) {
		wand.inventory.clear();
		for (auto spell : spells) {
			wand.inventory.emplace_back(spell);
		}
	});

	widget->addToTop(std::move(inventory1));

	auto inventory2 = Util::makeUnique(new NSpellInventory({350, 50}, 5));
	widget->addToTop(std::move(inventory2));

	constexpr auto windowSize = NScale::defaultWindowSizeF;
	auto healthBar =
		Util::makeUnique(new ValueBar(
			{windowSize.x - 20, 20},
			{320, 22}, 10, 2.f, ValueBar::HEALTH));
	healthBar->setHealth(10);
	widget->addToTop(std::move(healthBar));

	expBar =
		new ValueBar({windowSize.x - 20, 20 + 22 + 20}, {320, 22},
					 20, 0.f, ValueBar::EXP);
	expBar->setLevel(game.state.player.level);
	expBar->setHealth(game.state.player.exp);
	widget->add(Util::makeUnique(expBar));

	materialBar = new MaterialBar({windowSize.x - 20, 20 + 22 + 20 + 22 + 20}, 27.f);
	materialBar->setData(100);
	widget->add(Util::makeUnique(materialBar));

	// spellSelector = new NSpellSelector(windowSize / 2.f, 3, widget);

	pauseText = new NText("游戏暂停", NText::Center, 30U);
	pauseText->setGeometry({0, 0, windowSize.x, windowSize.y / 4.f});
	pauseText->isVisible = false;
	widget->add(Util::makeUnique(pauseText));
}
