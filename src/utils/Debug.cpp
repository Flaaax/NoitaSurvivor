#include "Debug.h"

#include <windows.h>
#include <dbghelp.h>
#include <iostream>
#include "Logger.h"

#pragma comment(lib, "dbghelp.lib")

void Util::Debug::printCallStack() {
	// 初始化符号库（进程句柄、是否仅本地符号）
	HANDLE hProcess = GetCurrentProcess();
	SymInitialize(hProcess, NULL, TRUE);

	// 栈帧存储
	const DWORD MAX_STACK_FRAMES = 32;
	void* stackFrames[MAX_STACK_FRAMES];
	DWORD frameCount = CaptureStackBackTrace(0, MAX_STACK_FRAMES, stackFrames, NULL);

	std::cout << "\n调用堆栈（共" << frameCount << "帧）：\n";
	for (DWORD i = 0; i < frameCount; i++) {
		DWORD64 addr = (DWORD64)stackFrames[i];
		CHAR buffer[sizeof(SYMBOL_INFO) + 256]{};
		PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;
		pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
		pSymbol->MaxNameLen = 255;

		// 解析地址为符号（函数名）
		if (SymFromAddr(hProcess, addr, NULL, pSymbol)) {
			std::cout << "  [" << i << "] " << pSymbol->Name
				<< " (0x" << std::hex << addr << std::dec << ")\n";
		}
		else {
			// 解析失败时显示地址
			std::cout << "  [" << i << "] 0x" << std::hex << addr << std::dec << "\n";
		}
	}
	std::cout << std::endl;
	// 清理符号库
	SymCleanup(hProcess);
}