#define WIN_32_LEAN_AND_MEAN
#include <iostream>
#include <Windows.h>
#include <cstdint>
#include <thread>
#include "functions.h"

namespace offset
{
	constexpr uintptr_t posHead = 0x4;
	constexpr uintptr_t posBody = 0x28;
	constexpr uintptr_t viewAngles = 0x34;
	constexpr uintptr_t noClip = 0x76;
	constexpr uintptr_t health = 0xEC;
	constexpr uintptr_t armor = 0xF0;
	constexpr uintptr_t pistolAmmo = 0x108;
	constexpr uintptr_t arAmmo = 0x11C;
	constexpr uintptr_t pistolClip = 0x12C;
	constexpr uintptr_t arClip = 0x140;
	constexpr uintptr_t isKnifing = 0x14C;
	constexpr uintptr_t pistolDelay = 0x150;
	constexpr uintptr_t arDelay = 0x164;
}

// global flag
uintptr_t baseAddress = (uintptr_t)GetModuleHandleA("ac_client.exe");
uintptr_t playerBase = *reinterpret_cast<uintptr_t*>(baseAddress + 0x17E0A8);
bool infiniteAmmoEnabled = false;
bool noClipEnabled = false;
char ammoOriginalBytes[2];
char ammoOpCode[] = { 0x90, 0x90 };
uintptr_t ammoFunc = baseAddress + 0xC73EF;
bool thread1Running = true;
bool thread2Running = true;

void Initialization(HMODULE instance) noexcept
{
	// Allocates console for debugging
	AllocConsole();
	FILE* f = nullptr;
	freopen_s(&f, "CONOUT$", "w", stdout);
	std::cout << "Injected! Debugging active...\n";
	memcpy(ammoOriginalBytes, (void*)ammoFunc, 2);


	// wait for uninjection process
	while (!GetAsyncKeyState(VK_NUMPAD0))
	{
		Sleep(10);
	}

	// Cleanup and exit
	Sleep(100);
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
	while (!GetAsyncKeyState(VK_NUMPAD0))
	{
		Sleep(10);

		if (GetAsyncKeyState(VK_NUMPAD1) & 1)
		{
			infiniteAmmoEnabled = !infiniteAmmoEnabled;

			if (infiniteAmmoEnabled)
			{
				WriteToMemory(ammoFunc, ammoOpCode, 2);
				std::cout << "Infinite Ammo ENABLED!\n";
			}
			else
			{
				WriteToMemory(ammoFunc, ammoOriginalBytes, 2);
				std::cout << "Infinite Ammo DISABLED!\n";
			}
		}
		else if (GetAsyncKeyState(VK_NUMPAD2) & 1)
		{
			noClipEnabled = !noClipEnabled;
			int16_t* noClip = reinterpret_cast<int16_t*>((uintptr_t)playerBase + offset::noClip);

			if (noClipEnabled)
			{
				__try
				{
					*noClip = 4;
					std::cout << "NoClip ENABLED!\n";
				}
				__except (EXCEPTION_EXECUTE_HANDLER)
				{
					std::cout << "Pointer dereference failed!\n";
				}
			}
			else
			{
				*noClip = 0;
				std::cout << "NoClip DISABLED!\n";
			}
		}
	}
	
	thread1Running = false;
}

void FutureThread(HMODULE instance) noexcept
{
	// code
	while (!GetAsyncKeyState(VK_NUMPAD0))
	{
		Sleep(10);
	}

	thread2Running = false;
}

int __stdcall DllMain(HMODULE instance, std::uintptr_t reason, const void* reserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(instance);
		const auto thread = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(Initialization), instance, 0, nullptr);
		if (thread) CloseHandle(thread);
		const auto thread2 = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(OverwriteValues), instance, 0, nullptr);
		if (thread2) CloseHandle(thread2);
		const auto thread3 = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(FutureThread), instance, 0, nullptr);
		if (thread3) CloseHandle(thread3);
		break;
	}

	return TRUE;
}