#pragma once
#include "../utils/Container/Map.h"
#include "src/utils/Container/Vector.h"
#include "src/utils/Pointer.h"

namespace flx::ui {
	struct WindowView;
	struct WindowEvent;
	class RenderBuffer;
} // namespace flx::ui

namespace flx::app {
	class Scene;

	struct SceneCmd {
		enum Cmd {
			None,
			Enter,
			Exit,
			ExitAll,
			UpdateLayer,
		};

		std::string target;
		Cmd cmd{};
	};

	class SceneManager {
	private:
		StrMap<SUnique<Scene>> scenes;
		Vector<SWeak<Scene>> activeScenes;
		Vector<SceneCmd> commands;
		// SWeak<Scene> sceneToChange{};

		void sort();

		// Commands
		void enter(std::string_view name);
		void exit(std::string_view name);
		void exitAll();
		SWeak<Scene> addInternal(SUnique<Scene> scene);

	public:
		SceneManager();
		~SceneManager();

		template <std::derived_from<Scene> T>
		SWeak<T> add(SUnique<T> scene) {
			return this->addInternal(scene | move).template staticCast<T>();
		}

		SWeak<Scene> get(std::string_view name, bool required = true) const;
		bool isActive(std::string_view name) const;

		void addCommand(SceneCmd cmd);

		// Call at the start of a frame
		void handleCommands();

		void update(float dt);
		void draw(ui::RenderBuffer& buffer) const;
		bool handleEvent(const ui::WindowEvent& event) const;
		void onWindowResized() const;
		void makeImGuiContent();

		// void setCurrent(std::string_view name) {
		// 	sceneToChange = get(name);
		// }

		Span<const SWeak<Scene>> getActiveScenes() const {
			return activeScenes;
		}

		// bool shouldChangeScene() const {
		// 	return !sceneToChange.expired();
		// }

		// void changeScene();
	};
} // namespace flx::app