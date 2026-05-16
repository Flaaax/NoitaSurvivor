#include"MenuScene.h"
#include "../ui/NButton.h"
#include "src/ui/NWindow.h"

MenuScene::MenuScene() {
	constexpr auto windowSize = NWindow::scale.defaultWindowSizeF;
	const auto startButton = new NButton(nrect(windowSize.x / 2, windowSize.y / 2, 200, 100, nrect::CenterRect));
	startButton->setOnClick([] { 
		NWindow::sceneManager.setCurrentScene("GameScene");
	});
	startButton->setText("开始游戏");
	setWidget(new NWidget());
	m_widget->add(startButton);

}

void MenuScene::draw(Renderer& renderer) {
	renderer.clear({ 200,200,200 });
}

void MenuScene::update(float deltaTime) {
}
