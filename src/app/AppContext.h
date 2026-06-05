#pragma once

struct NInputState;
class NWindowViewport;
class NWindow;

namespace flx::app {
	class SceneManager;

	struct AppContext {
		const NWindowViewport& windowViewport;
		const NInputState& input;
		SceneManager& sceneManager;
	};
}

