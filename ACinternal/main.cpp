#define WIN_32_LEAN_AND_MEAN
#include <iostream>
#include <Windows.h>
#include <cstdint>
#include <thread>
#include "functions.h"

// global vars
uintptr_t baseAddress = (uintptr_t)GetModuleHandleA("ac_client.exe");
uintptr_t playerBase = *reinterpret_cast<uintptr_t*>(baseAddress + 0x17E0A8);
uintptr_t entityList = *reinterpret_cast<uintptr_t*>(baseAddress + 0x18AC04);
uintptr_t currentWeapon = *reinterpret_cast<uintptr_t*>(playerBase + 0x368);
uintptr_t currentWeaponStats = *reinterpret_cast<uintptr_t*>(currentWeapon + 0x10);

uintptr_t ammoFunc = baseAddress + 0xC73EF;
char ammoOriginalBytes[2];
char ammoOpCode[] = { 0x90, 0x90 };

uintptr_t recoilFunc = baseAddress + 0xC2EC3;
char recoilOriginalBytes[5];
char recoilOpCode[] = { 0x90, 0x90, 0x90, 0x90, 0x90 };

uintptr_t kickbackFunc = baseAddress + 0xC0ACF;
char kickbackOriginalBytes[4];
char kickbackOpCode[] = { 0x90, 0x90, 0x90, 0x90 };

bool infiniteAmmoEnabled = false;
bool noclipEnabled = false;
bool godmodeEnabled = false;
bool rapidFireEnabled = false;
bool noRecoilEnabled = false;
bool noSpreadEnabled = false;
bool thread1Running = true;
bool thread2Running = true;
bool thread3Running = true;

void Initialization(HMODULE instance) noexcept
{
	// Allocates console for debugging
	AllocConsole();
	FILE* f = nullptr;
	freopen_s(&f, "CONOUT$", "w", stdout);
	showMenu(infiniteAmmoEnabled, noclipEnabled, godmodeEnabled, rapidFireEnabled, noRecoilEnabled);
	memcpy(ammoOriginalBytes, (void*)ammoFunc, 2);
	memcpy(recoilOriginalBytes, (void*)recoilFunc, 5);


	// wait for uninjection process
	while (!GetAsyncKeyState(VK_NUMPAD0))
	{
		Sleep(5);
	}

	// Cleanup and exit
	Sleep(100);
	while (!CanUninject(thread1Running, thread2Running, thread3Running))
	{
		std::cout << "Cannot uninject yet! Threads still running.\n";
		Sleep(100);
	}
	if (f) fclose(f);
	FreeConsole();
	FreeLibraryAndExitThread(instance, 0);
}

void OverwriteValues(HMODULE instance) noexcept 
{
	while (!GetAsyncKeyState(VK_NUMPAD0))
	{
		Sleep(5);

		if (GetAsyncKeyState(VK_NUMPAD1) & 1)
		{
			infiniteAmmoEnabled = !infiniteAmmoEnabled;

			if (infiniteAmmoEnabled)
			{
				WriteToMemory(ammoFunc, ammoOpCode, 2);
				system("cls");
				showMenu(infiniteAmmoEnabled, noclipEnabled, godmodeEnabled, rapidFireEnabled, noRecoilEnabled);
			}
			else
			{
				WriteToMemory(ammoFunc, ammoOriginalBytes, 2);
				system("cls");
				showMenu(infiniteAmmoEnabled, noclipEnabled, godmodeEnabled, rapidFireEnabled, noRecoilEnabled);
			}
		}
		else if (GetAsyncKeyState(VK_NUMPAD2) & 1)
		{
			noclipEnabled = !noclipEnabled;
			int16_t* noClip = reinterpret_cast<int16_t*>((uintptr_t)playerBase + offset::noClip);

			if (noclipEnabled)
			{
				__try
				{
					*noClip = 4;
					system("cls");
					showMenu(infiniteAmmoEnabled, noclipEnabled, godmodeEnabled, rapidFireEnabled, noRecoilEnabled);
				}
				__except (EXCEPTION_EXECUTE_HANDLER)
				{
					std::cout << "Pointer dereference failed!\n";
				}
			}
			else
			{
				*noClip = 0;
				system("cls");
				showMenu(infiniteAmmoEnabled, noclipEnabled, godmodeEnabled, rapidFireEnabled, noRecoilEnabled);
			}
		}
		else if (GetAsyncKeyState(VK_NUMPAD5) & 1)
		{
			noRecoilEnabled = !noRecoilEnabled;

			if (noRecoilEnabled)
			{
				WriteToMemory(recoilFunc, recoilOpCode, 5);
				system("cls");
				showMenu(infiniteAmmoEnabled, noclipEnabled, godmodeEnabled, rapidFireEnabled, noRecoilEnabled);
			}
			else
			{
				WriteToMemory(recoilFunc, recoilOriginalBytes, 5);
				system("cls");
				showMenu(infiniteAmmoEnabled, noclipEnabled, godmodeEnabled, rapidFireEnabled, noRecoilEnabled);
			}
		}
		else if (GetAsyncKeyState(VK_NUMPAD6) & 1)
		{
			// no spread hack
		}
	}
	
	thread1Running = false;
}

void Godmode(HMODULE instance) noexcept
{
	while (!GetAsyncKeyState(VK_NUMPAD0))
	{
		Sleep(5);

		if (GetAsyncKeyState(VK_NUMPAD3) & 1)
		{
			ToggleGodmode();
			if (godmodeEnabled)
				MaintainGodmode();
		}
	}

	thread2Running = false;
}

void RapidFire(HMODULE instance) noexcept
{
	while (!GetAsyncKeyState(VK_NUMPAD0))
	{
		Sleep(5);
		if (GetAsyncKeyState(VK_NUMPAD4) & 1)
		{
			ToggleRapidFire();
			if (rapidFireEnabled)
				MaintainRapidFire();
		}
	}

	thread3Running = false;
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
		const auto thread3 = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(Godmode), instance, 0, nullptr);
		if (thread3) CloseHandle(thread3);
		const auto thread4 = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(RapidFire), instance, 0, nullptr);
		if (thread4) CloseHandle(thread4);
		break;
	}

	return TRUE;
}