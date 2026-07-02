#include "MenuScene.h"
#include "../ui/elements/Button.h"
#include "src/app/SceneManager.h"
#include "src/ui/render/RenderBuffer.h"

namespace flx::app {
	MenuScene::MenuScene(AppCtx context) : Scene(context, makeContentID<MenuScene>()) {
		const vec2 windowSize = context.window.getView().canvasSize;
		auto startButton =
			makeUnique(new ui::Button(rect::fromCenter(windowSize / 2.f, {200, 100})));
		startButton->setOnClick([this] {
			// this->nextScene = "game_scene";
			// this->ctx.sceneManager.setCurrent("game_scene");
			this->ctx.sceneManager.addCommand({name, SceneCmd::Exit});
			this->ctx.sceneManager.addCommand({"game_scene", SceneCmd::Enter});
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