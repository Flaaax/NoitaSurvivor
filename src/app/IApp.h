#pragma once
#include "src/utils/Vec2/Vec2.h"

namespace flx::app {
	struct AppCtx;

	struct AppInfo {
		std::string name = "Unnamed App";
		vec2u defaultWindowSize = {1920, 1080};
		std::string defaultFont = "";
		bool imguiEnabled{};
	};

	struct IApp {
		virtual ~IApp() = default;
		virtual void setup(AppCtx context) = 0;
		virtual AppInfo getInfo() = 0;
	};
} // namespace flx::app
