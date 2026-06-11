#pragma once

struct NInputState;
class NWindowView;
class NWindow;

namespace flx::app {
	class SceneManager;

	struct AppContext {
		const NWindowView& windowViewport;
		const NInputState& input;
		SceneManager& sceneManager;
	};
}

