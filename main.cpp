#pragma warning(disable : 5260)
#include "src/app/Application.h"
#include "src/game/app/NoitaSurvivorApp.h"

int main() {
	// Logger::info("------Application started------\n");

	const flx::app::AppInit init{
		.name = "NoitaSurvivor",
		.imguiEnabled = true,
		.displayDebugFPS = true,
	};

	flx::app::Application::run<flx::app::NoitaSurvivorApp>(init);

	return 1;
}