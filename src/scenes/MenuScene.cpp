#include "MenuScene.h"
#include "../ui/NButton.h"
#include "src/ui/NWindow.h"

MenuScene::MenuScene() {
	constexpr nvec2 windowSize = NWindow::scale.defaultWindowSizeF;
	auto startButton =
		Util::makeUnique(new NButton(nrect(windowSize.x / 2, windowSize.y / 2, 200, 100, nrect::CenterRect)));
	startButton->setOnClick([] {
		NWindow::sceneManager.setCurrentScene("GameScene");
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
