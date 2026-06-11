#include "MenuScene.h"
#include "../ui/elements/NButton.h"
#include "src/app/SceneManager.h"
#include "src/render/Renderer.h"
#include "src/ui/NWindow.h"
#include "src/ui/render/NRenderBuffer.h"

MenuScene::MenuScene(flx::app::AppContext context) : Scene(context, Util::makeContentID<MenuScene>()) {
	const nvec2 windowSize = context.windowViewport.canvasSize;
	auto startButton =
		Util::makeUnique(new NButton(nrect::fromCenter(windowSize / 2.f, {200, 100})));
	startButton->setOnClick([this] {
		//this->nextScene = "game_scene";
		this->context.sceneManager.setCurrent("game_scene");
	});
	startButton->setText("开始游戏");
	createWidget();
	widget->addToTop(std::move(startButton));
}

void MenuScene::draw(NRenderBuffer& rdr) {
	rdr.clear({200, 200, 200});
	Scene::draw(rdr);
}

void MenuScene::update(float dt) {
}
