#pragma once
#include "AppContext.h"
#include "IApp.h"
#include "SceneManager.h"
#include "src/ui/NWindow.h"
#include "src/utils/Logging/Logger.h"

namespace flx::app {

	struct AppInit {
		std::string name = "Unnamed App";
		vec2u defaultWindowSize = {1920, 1080};
		bool imguiEnabled{};
		std::string imGuiFontPath = "./resources/fonts/msyh.ttc";
		bool displayDebugFPS{};
	};

	class Application {
	private:
		ui::NWindow window;
		SceneManager sceneManager;
		Logger logger;
		bool imguiEnabled{};
		bool showDebugFPS{};
		bool runGuard{};

		int loop();
		AppContext getContext();
		explicit Application(const AppInit& info);

	public:
		template <std::derived_from<IApp> MyApp>
		static int run(AppInit info) {
			MyApp app1;
			Application app(info);
			app1.setup(app.getContext());
			return app.loop();
		}
	};
} // namespace flx::app
