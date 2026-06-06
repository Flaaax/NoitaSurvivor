#pragma once

namespace flx::app {
	struct AppContext;

	struct IApp {
		virtual ~IApp() = default;
		virtual void setup(AppContext context) = 0;
	};
} // namespace flx::app
