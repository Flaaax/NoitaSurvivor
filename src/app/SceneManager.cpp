#include "SceneManager.h"
#include "Scene.h"
#include "src/utils/Assert.h"

namespace flx::app {
	static Logger logger = Logger::makeAsync("SceneManager");

	void SceneManager::add(SUnique<Scene> scene) {
		// Logger::info("Adding scene to SceneManager...");
		if (!scene) {
			logger.error_and_throw("Scene is nullptr");
		}
		const auto& name = scene->name;
		if (scenes.contains(name)) {
			logger.error_and_throw("Scene {} already registered!", name);
		}
		scenes[name] = std::move(scene);
		// Logger::info("Scene {} added to SceneManager", name);
	}

	SWeak<Scene> SceneManager::get(std::string_view name) const {
		if (const auto ret = scenes.try_find(name)) {
			return *ret;
		}
		logger.error_and_throw("Scene {} not registered!", name);
	}

	SceneManager::SceneManager() {}

	SceneManager::~SceneManager() {
		if (currentScene) {
			currentScene->exit();
		}
	}

	void SceneManager::changeScene() {
		if (!sceneToChange) {
			logger.error_and_throw("No scene to change");
		}
		if (currentScene) {
			currentScene->exit();
		}
		currentScene = sceneToChange;
		currentScene->enter();
		sceneToChange = {};
	}

	void SceneManager::exitAll() const {
		if (currentScene) {
			currentScene->exit();
		}
	}

} // namespace flx::app
