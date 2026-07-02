#pragma once
#include "AppCmd.h"
#include "src/utils/Container/Vector.h"

namespace flx::ui {
	struct InputState;
	struct WindowView;
	class Window;
} // namespace flx::ui

namespace flx::app {
	class SceneManager;

	struct RuntimeCtx {
		bool showDebugFPS{};
		bool imguiDisplay{};
		std::string imguiTitle = "ImGui";
		Vector<AppCmd::Cmd> cmds{};
	};

	struct AppCtx {
		const ui::Window& window;
		const ui::InputState& input;
		SceneManager& sceneManager;
		RuntimeCtx& runtime;
	};
} // namespace flx::app
