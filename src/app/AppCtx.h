#pragma once

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
	};

	struct AppCtx {
		const ui::WindowView& windowView;
		const ui::InputState& input;
		SceneManager& sceneManager;
		RuntimeCtx& runtime;
	};
} // namespace flx::app
