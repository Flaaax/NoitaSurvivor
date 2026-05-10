#include"GameScene.h"
#include"../gui/NButton.h"
#include"../gui/HealthBar.h"
#include"../gui/MaterialBar.h"
#include"src/game/Game.h"
#include"src/gui/game/SpellInventory.h"
#include"src/game/Wands/Wand.h"
#include"src/gui/game/SpellSelector.h"
#include"src/gui/NText.h"
#include <src/utils/Logger.h>


NText* pauseText = nullptr;
MaterialBar* materialBar = nullptr;
HealthBar* expBar = nullptr;
//NSpellSelector* spellSelector = nullptr;

GameScene::GameScene()
	:game(Game::inst()), logger(LoggerFactory::createAsync("GameScene")) {
	Logger::info("GameScene created");
}

GameScene::~GameScene() {
}

void GameScene::draw(Renderer& renderer) {
	game.draw(renderer);

	pauseText->setVisible(game.isPaused());
}

void GameScene::update(float deltaTime) {
	//if (spellSelector && !spellSelector->getIsRunning()) {
	//	m_widget->destroy(spellSelector);
	//	spellSelector = nullptr;
	//}

	game.update(deltaTime);
	auto& p = game.state.player;
	materialBar->setData(p.material);
	expBar->setValue(p.exp, p.maxExp, p.level);
}

void GameScene::handleEvent(const sf::Event& event) {
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Space) {
			game.setPaused(!game.isPaused());
		}
	}

	game.handleEvent(event);
}

void GameScene::enter() {
	init();
}

void GameScene::exit() {
}

void GameScene::onInit() {
	game.init();

	initUI();

	Logger::info("game initialized");
}

void GameScene::initUI() {
	auto widget = new NWidget();
	setWidget(widget);
	auto& wand = *game.state.wands.front();

	auto inventory1 = new NSpellInventory(0, { 100, 150 });
	widget->add(inventory1);
	inventory1->setFrom(wand.inventory);
	inventory1->setOnModify([&](const NSpellInventory& inv) {
		wand.clear();
		wand.inventory.clear();
		for (size_t i = 0; i < inv.getCount(); i++) {
			wand.inventory.emplace_back(inv.getSpell(i));
		}
	});
	auto inventory2 = new NSpellInventory(5, { 350,50 });
	widget->add(inventory2);

	constexpr auto windowSize = NScale::defaultWindowSize;
	auto healthBar = new HealthBar({ windowSize.x - 20,20 }, { 320,22 }, 10, 2.f);
	healthBar->setHealth(10);
	widget->add(healthBar);

	expBar = new HealthBar({ windowSize.x - 20,20 + 22 + 20 }, { 320,22 }, 20, 0, HealthBar::EXP);
	expBar->setLevel(game.state.player.level);
	expBar->setHealth(game.state.player.exp);
	widget->add(expBar);

	materialBar = new MaterialBar({ windowSize.x - 20,20 + 22 + 20 + 22 + 20 }, 27);
	materialBar->setData(100);
	widget->add(materialBar);

	//spellSelector = new NSpellSelector(windowSize / 2.f, 3, widget);

	pauseText = new NText("ÓÎÏ·ÔÝÍ£", NText::Center, 30U);
	pauseText->setGeometry({ 0,0,windowSize.x,windowSize.y / 4.f });
	pauseText->setVisible(false);
	widget->add(pauseText);

	widget->setUpdate(true);
}
