#pragma once
#pragma once
#ifndef NSCENEMANAGER_H
#define NSCENEMANAGER_H
#include "../utils/Container/Map.h"

namespace flx::app {
	class Scene;

	class SceneManager {
	private:
		Util::StrMap<n_unique<Scene>> scenes;
		Scene* currentScene{};
		Scene* sceneToChange{};

	public:
		SceneManager();
		~SceneManager();

		void addScene(n_unique<Scene> scene);

		Scene* getScene(std::string_view name) const;

		void setCurrentScene(std::string_view name) {
			sceneToChange = getScene(name);
		}

		Scene* getCurrentScene() const {
			return currentScene;
		}

		bool shouldChangeScene() const {
			return sceneToChange != nullptr;
		}

		void changeScene();
	};
} // namespace flax::app

#endif