#include "Debug.h"

// clang-format off
#include <windows.h>
#include <dbghelp.h>
// clang-format on

#include <iostream>

#pragma comment(lib, "dbghelp.lib")

void Util::Debug::printCallStack() {
	// Initialize the symbol handler
	const HANDLE hProcess = GetCurrentProcess();
	SymInitialize(hProcess, nullptr, TRUE);

	// Stack frame storage
	const DWORD MAX_STACK_FRAMES = 32;
	void* stackFrames[MAX_STACK_FRAMES];
	const DWORD frameCount = CaptureStackBackTrace(0, MAX_STACK_FRAMES, stackFrames, nullptr);

	std::cout << "\nCall stack, " << frameCount << " frames:\n";
	for (DWORD i = 0; i < frameCount; i++) {
		const DWORD64 addr = reinterpret_cast<DWORD64>(stackFrames[i]);
		CHAR buffer[sizeof(SYMBOL_INFO) + 256]{};
		PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;
		pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
		pSymbol->MaxNameLen = 255;

		// Resolve address to symbol name
		if (SymFromAddr(hProcess, addr, nullptr, pSymbol)) {
			std::cout << "  [" << i << "] " << pSymbol->Name
					  << " (0x" << std::hex << addr << std::dec << ")\n";
		} else {
			// Show the address when symbol resolution fails
			std::cout << "  [" << i << "] 0x" << std::hex << addr << std::dec << "\n";
		}
	}
	std::cout << std::endl;
	// Clean up the symbol handler
	SymCleanup(hProcess);
}
