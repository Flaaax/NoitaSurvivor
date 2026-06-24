#include "MenuScene.h"
#include "../ui/elements/NButton.h"
#include "src/app/SceneManager.h"
#include "src/ui/Window.h"
#include "src/ui/render/NRenderBuffer.h"

namespace flx::app {
	MenuScene::MenuScene(flx::app::AppCtx context) : Scene(context, flx::makeContentID<MenuScene>()) {
		const vec2 windowSize = context.windowViewport.canvasSize;
		auto startButton =
			flx::makeUnique(new ui::NButton(rect::fromCenter(windowSize / 2.f, {200, 100})));
		startButton->setOnClick([this] {
			// this->nextScene = "game_scene";
			this->context.sceneManager.setCurrent("game_scene");
		});
		startButton->setText("开始游戏");
		createWidget();
		widget->addToTop(std::move(startButton));
	}

	void MenuScene::draw(ui::RenderBuffer& rdr) {
		rdr.clear({200, 200, 200});
		Scene::draw(rdr);
	}

	void MenuScene::update(float dt) {
	}
} // namespace flx::app