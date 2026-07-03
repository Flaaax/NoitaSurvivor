#include "SceneManager.h"
#include "Scene.h"
#include "src/utils/Assert.h"
#include "src/utils/Container/View.h"

#include <imgui-SFML.h>
#include <imgui.h>

namespace flx::app {
	static Logger logger = Logger::makeAsync("SceneManager");

	SceneManager::SceneManager() = default;

	SceneManager::~SceneManager() {
		exitAll();
	}

	SWeak<Scene> SceneManager::addInternal(SUnique<Scene> scene) {
		// Logger::info("Adding scene to SceneManager...");
		if (!scene) {
			logger.error_and_throw("Scene is nullptr");
		}
		const auto& name = scene->name;
		if (scenes.contains(name)) {
			logger.error_and_throw("Scene {} already registered!", name);
		}
		return scenes[name] = std::move(scene);
		// Logger::info("Scene {} added to SceneManager", name);
	}

	SWeak<Scene> SceneManager::get(std::string_view name, bool required) const {
		if (const auto ret = scenes.try_find(name)) {
			return *ret;
		}
		if (required) {
			logger.error_and_throw("Scene {} not registered!", name);
		}
		return {};
	}

	bool SceneManager::isActive(std::string_view name) const {
		return view::all(activeScenes)
			.any([name](const SWeak<Scene>& scene) { return scene->name == name; });
	}

	void SceneManager::addCommand(SceneCmd cmd) {
		commands.emplace_back(std::move(cmd));
	}

	void SceneManager::handleCommands() {
		for (const auto& cmd : commands) {
			if (cmd.cmd == SceneCmd::None) {
				logger.error_and_throw("Command not specified");
			}
			if (cmd.cmd == SceneCmd::Exit) {
				exit(cmd.target);
			} else if (cmd.cmd == SceneCmd::ExitAll) {
				exitAll();
			} else if (cmd.cmd == SceneCmd::Enter) {
				enter(cmd.target);
			} else if (cmd.cmd == SceneCmd::UpdateLayer) {
				sort();
			}
		}
		commands.clear();
	}

	void SceneManager::update(float dt) {
		for (const auto& scene : activeScenes | std::views::reverse) {
			scene->update(dt);
		}
	}

	void SceneManager::draw(ui::RenderBuffer& buffer) const {
		for (const auto& scene : activeScenes) {
			scene->draw(buffer);
		}
	}

	bool SceneManager::handleEvent(const ui::WindowEvent& event) const {
		for (const auto& scene : activeScenes | std::views::reverse) {
			if (scene->handleEvent(event)) {
				return true;
			}
		}
		return false;
	}

	void SceneManager::onWindowResized() const {
		// for (const auto& scene : activeScenes | std::views::reverse) {
		// 	scene->onWindowResized();
		// }
		for (const auto& scene : scenes | std::views::values) {
			scene->onWindowResized();
		}
	}

	void SceneManager::makeImGuiContent() {
		for (const auto& scene : activeScenes | std::views::reverse) {
			ImGui::PushID(scene->name.c_str());
			scene->makeImGuiContent();
			ImGui::PopID();
		}
	}

	// void SceneManager::changeScene() {
	// 	if (!sceneToChange) {
	// 		logger.error_and_throw("No scene to change");
	// 	}
	// 	if (currentScene) {
	// 		currentScene->exit();
	// 	}
	// 	currentScene = sceneToChange;
	// 	currentScene->enter();
	// 	sceneToChange = {};
	// }

	void SceneManager::sort() {
		std::ranges::sort(activeScenes, {}, &Scene::getLayer);
	}

	void SceneManager::enter(std::string_view name) {
		if (isActive(name)) {
			logger.error_and_throw("Scene {} already active!", name);
		}
		auto s = get(name);
		s->enter();
		activeScenes.emplace_back(std::move(s));
		sort();
	}

	void SceneManager::exit(std::string_view name) {
		const auto it =
			std::ranges::find_if(activeScenes, [name](const SWeak<Scene>& scene) { return scene->name == name; });
		if (it == activeScenes.end()) {
			logger.error_and_throw("Scene {} not registered!", name);
		}
		it->get()->exit();
		activeScenes.erase(it);
		// No need to sort...
	}

	void SceneManager::exitAll() {
		for (const auto& scene : activeScenes) {
			scene->exit();
		}
		activeScenes = {};
	}

} // namespace flx::app
