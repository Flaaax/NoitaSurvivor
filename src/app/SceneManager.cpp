#include "SceneManager.h"
#include "Scene.h"
#include "src/utils/Assert.h"
#include <format>

namespace flx::app {
	void SceneManager::add(n_unique<Scene> scene) {
		// Logger::info("Adding scene to SceneManager...");
		if (!scene)
			throw std::runtime_error("scene is nullptr");
		const auto& name = scene->name;
		if (scenes.contains(name))
			return;
		scenes[name] = std::move(scene);
		// Logger::info("Scene {} added to SceneManager", name);
	}

	Scene* SceneManager::get(std::string_view name) const {
		if (const auto it = scenes.find(name); it != scenes.end()) {
			return it->second.get();
		}
		throw std::runtime_error(std::format("Scene {} does not exist or not registered!", name));
	}

	SceneManager::SceneManager() {}

	SceneManager::~SceneManager() {
		if (currentScene) {
			currentScene->exit();
		}
	}

	void SceneManager::changeScene() {
		if (currentScene) {
			currentScene->exit();
		}
		currentScene = sceneToChange;
		currentScene->enter();
		sceneToChange = nullptr;
	}

} // namespace flx::app
