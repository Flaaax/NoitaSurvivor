#pragma warning(disable : 5260)
#include "src/app/Application.h"
#include "src/game/app/NoitaSurvivorApp.h"
#include "src/utils/Exception/Exceptions.h"

// static void f() {
// 	std::string s = "hello";
// 	const auto t = std::move(s);
// 	flx::logger.info(t);
// 	s += "aaa";
// 	flx::logger.info(s);
// }

int main() {
	// Logger::info("------Application started------\n");

	using namespace flx;

	try {
		// const flx::app::AppInfo init{
		// 	.name = "NoitaSurvivor",
		// 	.imguiEnabled = true,
		// 	.displayDebugFPS = true,
		// };

		return app::Application::run<app::NoitaSurvivorApp>();
	} catch (const except::LogThrow&) {
	} catch (const std::exception& e) {
		logger.error("Unknown error: {}", e.what());
	}

	return 1;
}