#pragma warning(disable:5260)
#include "src/ui/NWindow.h"

int main() {
	Logger::info("------Application started------\n");
    return NWindow::inst().loop();
	try {
		return NWindow::inst().loop();
	}
	catch (const std::exception& e) {
		Logger::error(e.what());
	}
	return 1;
}