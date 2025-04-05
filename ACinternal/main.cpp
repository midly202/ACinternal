#define WIN_32_LEAN_AND_MEAN
#include <iostream>
#include <Windows.h>
#include <cstdint>
#include <thread>
#include "functions.h"

// global flag
uintptr_t baseAddress = (uintptr_t)GetModuleHandleA("ac_client.exe");
bool thread1Running = true;
bool thread2Running = true;
bool thread3Running = true;

void DetectKeypress(HMODULE instance) noexcept
{
	// Allocates console for debugging
	AllocConsole();
	FILE* f = nullptr;
	freopen_s(&f, "CONOUT$", "w", stdout);
	std::cout << "Injected! Debugging active...\n";
	std::cout << "DetectKeypress thread active...\n";

	while (!GetAsyncKeyState(VK_NUMPAD0))
	{
		Sleep(10);
		// code to detect keypresses
	}

	// Cleanup and exit
	while (!GetAsyncKeyState(VK_NUMPAD0))
		Sleep(10);

	thread1Running = false;
	Sleep(100);

	// only free the DLL if both threads are done
	while (!CanUninject(thread1Running, thread2Running))
	{
		std::cout << "Cannot uninject yet! Threads still running.\n";
		Sleep(100);
	}

	fclose(f);
	FreeConsole();
	FreeLibraryAndExitThread(instance, 0);
}

void OverwriteValues(HMODULE instance) noexcept 
{
	int counter = 0;

	std::cout << "OverwriteValues thread active...\n";

	while (!GetAsyncKeyState(VK_NUMPAD0))
	{
		Sleep(50);
		std::cout << counter << "\n";
		counter++;
	}
	
	thread2Running = false;
}

int __stdcall DllMain(HMODULE instance, std::uintptr_t reason, const void* reserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(instance);
		const auto thread = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(DetectKeypress), instance, 0, nullptr);
		if (thread) CloseHandle(thread);
		const auto thread2 = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(OverwriteValues), instance, 0, nullptr);
		if (thread2) CloseHandle(thread2);
		break;
	}

	return TRUE;
}