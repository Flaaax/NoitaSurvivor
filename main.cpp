#pragma warning(disable:5260)
#include"src/gui/NWindow.h"

int main() {
	Logger::info("------Application started------\n");
	return NWindow::inst().loop();
	return 1;
}