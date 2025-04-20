#define WIN_32_LEAN_AND_MEAN
#include <iostream>
#include <Windows.h>
#include <cstdint>
#include <thread>
#include "functions.h"

// global vars
uintptr_t baseAddress = (uintptr_t)GetModuleHandleA("ac_client.exe");
ent* player = NULL;
gameInfo* game = NULL;
CGameMode* gameMode = NULL;
currWeapon* currentWeapon = NULL;
weaponStatsDynamic* currentWeaponStatsDynamic = NULL;
weaponStatsStatic* currentWeaponStatsStatic = NULL;

uintptr_t ammoFunc = baseAddress + 0xC73EF;
char ammoOriginalBytes[2];
char ammoOpCode[] = { 0x90, 0x90 };

uintptr_t nadeFunc = baseAddress + 0xC7E5D;
char nadeOriginalBytes[2];
char nadeOpCode[] = { 0x90, 0x90 };

uintptr_t recoilFunc = baseAddress + 0xC2EC3;
char recoilOriginalBytes[5];
char recoilOpCode[] = { 0x90, 0x90, 0x90, 0x90, 0x90 };

bool infiniteAmmoEnabled = false;
bool noclipEnabled = false;
bool invisEnabled = false;
bool godmodeEnabled = false;
bool rapidFireEnabled = false;
bool noRecoilEnabled = false;
bool noKickbackEnabled = false;
bool noSpreadEnabled = false;
bool aimbotEnabled = false;
bool thread1Running = true;
bool thread2Running = true;
bool thread3Running = true;
bool thread4Running = true;
bool thread5Running = true;

void Initialization(HMODULE instance) noexcept
{
	AllocConsole();
	FILE* f = nullptr;
	freopen_s(&f, "CONOUT$", "w", stdout);
	system("cls");
	showMenu(infiniteAmmoEnabled, noclipEnabled, godmodeEnabled, rapidFireEnabled, noRecoilEnabled, noKickbackEnabled, invisEnabled, aimbotEnabled);
	memcpy(ammoOriginalBytes, (void*)ammoFunc, 2);
	memcpy(nadeOriginalBytes, (void*)nadeFunc, 2);
	memcpy(recoilOriginalBytes, (void*)recoilFunc, 5);


	// wait for uninjection process
	while (!GetAsyncKeyState(VK_NUMPAD0))
	{
		Sleep(5);
	}

	// Cleanup and exit
	Sleep(100);
	while (!CanUninject(thread1Running, thread2Running, thread3Running, thread4Running, thread5Running))
	{
		std::cout << "Cannot uninject yet! Threads still running.\n";
		Sleep(100);
	}
	if (f) fclose(f);
	FreeConsole();
	FreeLibraryAndExitThread(instance, 0);
}

void OverwriteOpcodes(HMODULE instance) noexcept 
{
	while (!GetAsyncKeyState(VK_NUMPAD0))
	{
		Sleep(5);
		// inf ammo
		if (GetAsyncKeyState(VK_NUMPAD1) & 1)
		{
			infiniteAmmoEnabled = !infiniteAmmoEnabled;

			if (infiniteAmmoEnabled)
			{
				WriteToMemory(ammoFunc, ammoOpCode, 2);
				WriteToMemory(nadeFunc, nadeOpCode, 2);
				system("cls");
				showMenu(infiniteAmmoEnabled, noclipEnabled, godmodeEnabled, rapidFireEnabled, noRecoilEnabled, noKickbackEnabled, invisEnabled, aimbotEnabled);
			}
			else
			{
				WriteToMemory(ammoFunc, ammoOriginalBytes, 2);
				WriteToMemory(nadeFunc, nadeOriginalBytes, 2);
				system("cls");
				showMenu(infiniteAmmoEnabled, noclipEnabled, godmodeEnabled, rapidFireEnabled, noRecoilEnabled, noKickbackEnabled, invisEnabled, aimbotEnabled);
			}
		}
		// noclip
		else if (GetAsyncKeyState(VK_NUMPAD2) & 1)
		{
			noclipEnabled = !noclipEnabled;

			DWORD* playerBasePtr = (DWORD*)(baseAddress + offset::playerBase);
			if (!playerBasePtr || !*playerBasePtr) continue;

			ent* player = (ent*)(*playerBasePtr);

			if (noclipEnabled)
			{
				__try
				{
					player->noClip = 4;
					system("cls");
					showMenu(infiniteAmmoEnabled, noclipEnabled, godmodeEnabled, rapidFireEnabled, noRecoilEnabled, noKickbackEnabled, invisEnabled, aimbotEnabled);
				}
				__except (EXCEPTION_EXECUTE_HANDLER)
				{
					std::cout << "Pointer dereference failed!\n";
				}
			}
			else
			{
				player->noClip = 0;
				system("cls");
				showMenu(infiniteAmmoEnabled, noclipEnabled, godmodeEnabled, rapidFireEnabled, noRecoilEnabled, noKickbackEnabled, invisEnabled, aimbotEnabled);
			}
		}
		// no recoil
		else if (GetAsyncKeyState(VK_NUMPAD5) & 1)
		{
			noRecoilEnabled = !noRecoilEnabled;

			if (noRecoilEnabled)
			{
				WriteToMemory(recoilFunc, recoilOpCode, 5);
				system("cls");
				showMenu(infiniteAmmoEnabled, noclipEnabled, godmodeEnabled, rapidFireEnabled, noRecoilEnabled, noKickbackEnabled, invisEnabled, aimbotEnabled);
			}
			else
			{
				WriteToMemory(recoilFunc, recoilOriginalBytes, 5);
				system("cls");
				showMenu(infiniteAmmoEnabled, noclipEnabled, godmodeEnabled, rapidFireEnabled, noRecoilEnabled, noKickbackEnabled, invisEnabled, aimbotEnabled);
			}
		}
		// yoru ult
		else if (GetAsyncKeyState(VK_NUMPAD7) & 1)
		{
			invisEnabled = !invisEnabled;

			DWORD* playerBasePtr = (DWORD*)(baseAddress + offset::playerBase);
			if (!playerBasePtr || !*playerBasePtr) continue;

			ent* player = (ent*)(*playerBasePtr);

			if (invisEnabled)
			{
				__try
				{
					player->noClip = 7;
					system("cls");
					showMenu(infiniteAmmoEnabled, noclipEnabled, godmodeEnabled, rapidFireEnabled, noRecoilEnabled, noKickbackEnabled, invisEnabled, aimbotEnabled);
				}
				__except (EXCEPTION_EXECUTE_HANDLER)
				{
					std::cout << "Pointer dereference failed!\n";
				}
			}
			else
			{
				player->noClip = 0;
				system("cls");
				showMenu(infiniteAmmoEnabled, noclipEnabled, godmodeEnabled, rapidFireEnabled, noRecoilEnabled, noKickbackEnabled, invisEnabled, aimbotEnabled);
			}
		}
	}
	
	thread1Running = false;
}

void NoKickback(HMODULE instance) noexcept
{
	while (!GetAsyncKeyState(VK_NUMPAD0))
	{
		Sleep(5);
		if (GetAsyncKeyState(VK_NUMPAD6) & 1)
		{
			ToggleNoKickback();
			if (noKickbackEnabled)
				MaintainNoKickback();
		}
	}
	thread2Running = false;
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

	thread3Running = false;
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

	thread4Running = false;
}

void Aimbot(HMODULE instance) noexcept
{
	while (!GetAsyncKeyState(VK_NUMPAD0))
	{
		Sleep(5);
		if (GetAsyncKeyState(VK_NUMPAD8) & 1)
		{
			ToggleAimbot();
			if (aimbotEnabled)
				MaintainAimbot();
		}
	}
	thread5Running = false;
}

int __stdcall DllMain(HMODULE instance, std::uintptr_t reason, const void* reserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(instance);
		const auto thread = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(Initialization), instance, 0, nullptr);
		if (thread) CloseHandle(thread);
		const auto thread2 = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(OverwriteOpcodes), instance, 0, nullptr);
		if (thread2) CloseHandle(thread2);
		const auto thread3 = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(NoKickback), instance, 0, nullptr);
		if (thread3) CloseHandle(thread3);
		const auto thread4 = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(Godmode), instance, 0, nullptr);
		if (thread4) CloseHandle(thread4);
		const auto thread5 = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(RapidFire), instance, 0, nullptr);
		if (thread5) CloseHandle(thread5);
		const auto thread6 = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(Aimbot), instance, 0, nullptr);
		if (thread6) CloseHandle(thread6);
		break;
	}

	return TRUE;
}