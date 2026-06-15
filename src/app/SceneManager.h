#pragma once
#pragma once
#ifndef NSCENEMANAGER_H
#define NSCENEMANAGER_H
#include "../utils/Container/Map.h"
#include "src/utils/Pointer.h"

namespace flx::app {
	class Scene;

	class SceneManager {
	private:
		flx::StrMap<Unique<Scene>> scenes;
		Scene* currentScene{};
		Scene* sceneToChange{};

	public:
		SceneManager();
		~SceneManager();

		void add(Unique<Scene> scene);

		Scene* get(std::string_view name) const;

		void setCurrent(std::string_view name) {
			sceneToChange = get(name);
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