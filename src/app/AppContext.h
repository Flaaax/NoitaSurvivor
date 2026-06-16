#pragma once

namespace flx::ui {
	struct NInputState;
	struct NWindowView;
	class NWindow;
} // namespace flx::ui

namespace flx::app {
	class SceneManager;

	struct AppContext {
		const ui::NWindowView& windowViewport;
		const ui::NInputState& input;
		SceneManager& sceneManager;
	};
} // namespace flx::app
