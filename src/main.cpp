#include "app/Application.h"
#include "game/app/NoitaSurvivorApp.h"
#include "utils/Exception/Exceptions.h"

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
		throw;
	} catch (const std::exception& e) {
		logger.error("Unknown error: {}", e.what());
	}

	return 1;
}