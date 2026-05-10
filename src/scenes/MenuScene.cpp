#include"MenuScene.h"
#include"../gui/NButton.h"
#include"src/gui/NWindow.h"

MenuScene::MenuScene() {
	auto windowSize = NWindow::scale.defaultWindowSize;
	auto startButton = new NButton(nrect(windowSize.x / 2, windowSize.y / 2, 200, 100, nrect::CenterRect));
	startButton->setOnClick([] { 
		NWindow::sceneManager.setCurrentScene("GameScene");
	});
	startButton->setText("¿ªÊ¼ÓÎÏ·");
	setWidget(new NWidget());
	m_widget->add(startButton);

}

void MenuScene::draw(Renderer& renderer) {
	renderer.clear({ 200,200,200 });
}

void MenuScene::update(float deltaTime) {
}
