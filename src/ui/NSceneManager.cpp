#include "NSceneManager.h"
#include "src/ui/NScene.h"
#include "src/utils/Assert.h"
#include <format>

void NSceneManager::addScene(n_unique<NScene> scene) {
	//Logger::info("Adding scene to SceneManager...");
	if (!scene)throw std::runtime_error("scene is nullptr");
	const auto& name = scene->getName();
	if (scenes.contains(name)) return;
	scenes[name] = std::move(scene);
	//Logger::info("Scene {} added to SceneManager", name);
}

NScene* NSceneManager::getScene(std::string_view name)const {
	if (auto it = scenes.find(name); it != scenes.end()) {
		return it->second.get();
	}
	throw std::runtime_error(std::format("Scene {} does not exist or not registered!", name));
}

NSceneManager::NSceneManager() {}

NSceneManager::~NSceneManager() {
	if (currentScene) {
		currentScene->exit();
	}
}

void NSceneManager::changeScene() {
	if (currentScene) {
		currentScene->exit();
	}
	currentScene = sceneToChange;
	currentScene->enter();
	sceneToChange = nullptr;
}
