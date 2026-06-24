#pragma once
#include "AppCtx.h"
#include "IApp.h"
#include "SceneManager.h"
#include "src/ui/Window.h"
#include "src/utils/Logging/Logger.h"

namespace flx::app {

	class Application {
	private:
		ui::Window window;
		SceneManager sceneManager;
		Logger logger;
		RuntimeCtx runtime;
		bool runGuard{};
		bool imguiEnabled{};
		std::string defaultFont;

		int loop();
		void initImgui();
		AppCtx getContext();
		bool shouldDisplayImgui()const;
		explicit Application(AppInfo info);
		~Application();

	public:
		template <std::derived_from<IApp> MyApp>
		static int run() {
			MyApp app1;
			Application app(app1.getInfo());
			app1.setup(app.getContext());
			return app.loop();
		}
	};
} // namespace flx::app
