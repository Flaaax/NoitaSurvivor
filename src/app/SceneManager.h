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
		StrMap<SUnique<Scene>> scenes;
		SWeak<Scene> currentScene{};
		SWeak<Scene> sceneToChange{};

	public:
		SceneManager();
		~SceneManager();

		void add(SUnique<Scene> scene);

		SWeak<Scene> get(std::string_view name) const;

		void setCurrent(std::string_view name) {
			sceneToChange = get(name);
		}

		SWeak<Scene> getCurrentScene() const {
			return currentScene;
		}

		bool shouldChangeScene() const {
			return !sceneToChange.expired();
		}

		void changeScene();

		void exitAll() const;
	};
} // namespace flax::app

#endif