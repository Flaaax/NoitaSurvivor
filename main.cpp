#pragma warning(disable : 5260)
#include "src/app/Application.h"
#include "src/game/app/NoitaSurvivorApp.h"
#include "src/utils/Exception/Exceptions.h"

int main() {
	// Logger::info("------Application started------\n");

	try {
		const flx::app::AppInit init{
			.name = "NoitaSurvivor",
			.imguiEnabled = true,
			.displayDebugFPS = true,
		};

		return flx::app::Application::run<flx::app::NoitaSurvivorApp>(init);
	} catch (flx::except::LogThrow) {
	} catch (const std::exception& e) {
		flx::logger.error("Unknown error: {}", e.what());
	}

	return 1;
}