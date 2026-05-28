#include "MenuScene.h"
#include "../ui/elements/NButton.h"
#include "src/render/Renderer.h"
#include "src/ui/NWindow.h"

MenuScene::MenuScene() {
	constexpr nvec2 windowSize = NWindow::scale.defaultWindowSizeF;
	auto startButton =
		Util::makeUnique(new NButton(nrect::fromCenter(windowSize / 2.f, {200, 100})));
	startButton->setOnClick([] {
		NWindow::sceneManager.setCurrentScene("game_scene");
	});
	startButton->setText("开始游戏");
	createWidget();
	widget->addToTop(std::move(startButton));
}

void MenuScene::draw(Renderer& rdr) {
	rdr.clear({200, 200, 200});
	NScene::draw(rdr);
}

void MenuScene::update(float dt) {
}

std::string_view MenuScene::getName() const {
	return NObject::makeTypeID<MenuScene>();
}
