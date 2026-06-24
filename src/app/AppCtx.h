#pragma once

namespace flx::ui {
	struct NInputState;
	struct NWindowView;
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
		const ui::NWindowView& windowViewport;
		const ui::NInputState& input;
		SceneManager& sceneManager;
		RuntimeCtx& runtime;
	};
} // namespace flx::app
