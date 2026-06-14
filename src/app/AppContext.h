#pragma once

namespace flx::ui {
	struct NInputState;
	struct NWindowView;
	class NWindow;

}

namespace flx::app {
	class SceneManager;

	struct AppContext {
		const ui::NWindowView& windowViewport;
		const ui::NInputState& input;
		SceneManager& sceneManager;
	};
}

