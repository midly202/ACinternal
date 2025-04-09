#include <iostream>
#include <Psapi.h>
#include <Windows.h>
#include "offsets.h"


extern bool godmodeEnabled;
extern bool infiniteAmmoEnabled;
extern bool lockInfiniteAmmo;
extern bool noclipEnabled;
extern bool invisEnabled;
extern bool rapidFireEnabled;
extern bool noRecoilEnabled;
extern bool noKickbackEnabled;
extern bool noKickbackLocked;
extern bool noSpreadEnabled;
extern uintptr_t playerBase;
extern uintptr_t entityList;
extern uintptr_t currentWeapon;
extern uintptr_t ammoFunc;
extern char ammoOpCode[];
extern char ammoOriginalBytes[];

void MsgBoxAddy(uintptr_t addy)
{
	char szBuffer[1024];
	sprintf_s(szBuffer, "Address: %08X", addy);
	MessageBoxA(NULL, szBuffer, "Address", MB_OK);
}

MODULEINFO GetModuleInfo(const char* szModule)
{
	MODULEINFO modInfo = { 0 };
	HMODULE hModule = GetModuleHandle(szModule);
	if (hModule == 0)
		return modInfo;
	GetModuleInformation(GetCurrentProcess(), hModule, &modInfo, sizeof(MODULEINFO));
	return modInfo;
}

void WriteToMemory(uintptr_t addressToWrite, char* valueToWrite, int byteNum)
{
	unsigned long oldProtection;
	if (!VirtualProtect((LPVOID)addressToWrite, byteNum, PAGE_EXECUTE_READWRITE, &oldProtection))
	{
		std::cerr << "Failed to change memory protection!" << std::endl;
		return;
	}

	memcpy((LPVOID)addressToWrite, valueToWrite, byteNum);

	unsigned long temp;
	if (!VirtualProtect((LPVOID)addressToWrite, byteNum, oldProtection, &temp))
	{
		std::cerr << "Failed to restore memory protection!" << std::endl;
	}
}

uintptr_t FindPattern(const char* module, const char* pattern, const char* mask)
{
	MODULEINFO moduleInfo = GetModuleInfo(module);
	uintptr_t baseAddress = (uintptr_t)moduleInfo.lpBaseOfDll;
	uintptr_t moduleSize = (uintptr_t)moduleInfo.SizeOfImage;
	uintptr_t patternLength = (uintptr_t)strlen(mask);
	for (uintptr_t i = 0; i < moduleSize - patternLength; i++)
	{
		bool found = true;
		for (uintptr_t j = 0; j < patternLength; j++)
		{
			found &= mask[j] == '?' || pattern[j] == *(char*)(baseAddress + i + j);
		}
		if (found)
		{
			return baseAddress + i;
		}
	}
	return NULL;
}

void PlaceJMP(BYTE* address, uintptr_t jumpTo, uintptr_t length)
{
	DWORD dwOldProtect, dwBkup;
	uintptr_t dwRelAddr;
	VirtualProtect(address, length, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	dwRelAddr = (uintptr_t)(jumpTo - (uintptr_t)address) - 5;
	*address = 0xE9;
	*((uintptr_t*)(address + 0x1)) = dwRelAddr;

	for (uintptr_t x = 0x5; x < length; x++)
	{
		*(address + x) = 0x90;
	}

	VirtualProtect(address, length, dwOldProtect, &dwBkup);
}

bool CanUninject(bool thread1Running, bool thread2Running, bool thread3running, bool thread4Running)
{
	if (thread1Running || thread2Running || thread3running || thread4Running)
		return false; // Returns false if a thread is still running
	else
		return true; // Returns true if all threads have exited
}

void showMenu(bool infiniteAmmo, bool noClip, bool Godmode, bool rapidFireEnabled, bool noRecoilEnabled, bool noKickbackEnabled, bool noKickbackLocked, bool invisEnabled)
{
	std::cout << "[1] Infinite Ammo [" << (infiniteAmmo ? "ON" : "OFF") << "]\n";
	std::cout << "[2] NoClip [" << (noClip ? "ON" : "OFF") << "]\n";
	std::cout << "[3] Godmode [" << (Godmode ? "ON" : "OFF") << "]\n";
	std::cout << "[4] Rapid Fire [" << (rapidFireEnabled ? "ON" : "OFF") << "]\n";
	std::cout << "[5] No Recoil [" << (noRecoilEnabled ? "ON" : "OFF") << "]\n";
	std::cout << "[6] No Kickback [" << (noKickbackEnabled ? "ON" : "OFF") << (noKickbackLocked ? "] [LOCKED]" : "]") << "\n";
	std::cout << "[7] Invisible [" << (invisEnabled ? "ON" : "OFF") << "] [Against Bots]\n";
}

void WaitForKeyRelease(int vkKey)
{
	while (GetAsyncKeyState(vkKey)) Sleep(10);
}

void ToggleInfiniteAmmo()
{
	infiniteAmmoEnabled = !infiniteAmmoEnabled;

	if (infiniteAmmoEnabled)
	{
		WriteToMemory(ammoFunc, ammoOpCode, 2);
	}
	else
	{
		WriteToMemory(ammoFunc, ammoOriginalBytes, 2);
	}

	WaitForKeyRelease(VK_NUMPAD1);
	system("cls");
	showMenu(infiniteAmmoEnabled, noclipEnabled, godmodeEnabled, rapidFireEnabled, noRecoilEnabled, noKickbackEnabled, noKickbackLocked, invisEnabled);
}

void MaintainInfiniteAmmo()
{
	while (infiniteAmmoEnabled && !GetAsyncKeyState(VK_NUMPAD0))
	{
		uintptr_t currentWeapon = *reinterpret_cast<uintptr_t*>(playerBase + 0x368);
		uintptr_t currentWeaponStatsStatic = *reinterpret_cast<uintptr_t*>(currentWeapon + 0xC);
		uintptr_t currentWeaponStatsDynamic = *reinterpret_cast<uintptr_t*>(currentWeapon + 0x10);

		int* ammo = reinterpret_cast<int*>(currentWeaponStatsDynamic + offset::weaponClip);
		*ammo = 999;
		Sleep(5);

		if (GetAsyncKeyState(VK_NUMPAD1) & 1)
		{
			infiniteAmmoEnabled = !infiniteAmmoEnabled;
			WaitForKeyRelease(VK_NUMPAD1);
			system("cls");
			showMenu(infiniteAmmoEnabled, noclipEnabled, godmodeEnabled, rapidFireEnabled, noRecoilEnabled, noKickbackEnabled, noKickbackLocked, invisEnabled);
			break;
		}
	}
}

void ToggleNoKickback()
{
	noKickbackEnabled = true;
	noKickbackLocked = true;
	WaitForKeyRelease(VK_NUMPAD6);
	system("cls");
	showMenu(infiniteAmmoEnabled, noclipEnabled, godmodeEnabled, rapidFireEnabled, noRecoilEnabled, noKickbackEnabled, noKickbackLocked, invisEnabled);
}

void MaintainNoKickback()
{
	while (noKickbackEnabled && !GetAsyncKeyState(VK_NUMPAD0))
	{
		// current weapon so these pointers change
		uintptr_t currentWeapon = *reinterpret_cast<uintptr_t*>(playerBase + 0x368);
		uintptr_t currentWeaponStatsStatic = *reinterpret_cast<uintptr_t*>(currentWeapon + 0xC);
		uintptr_t currentWeaponStatsDynamic = *reinterpret_cast<uintptr_t*>(currentWeapon + 0x10);

		int16_t* kickback = reinterpret_cast<int16_t*>(currentWeaponStatsStatic + offset::weaponKickback);
		*kickback = 0;
		Sleep(5);
	}
}

void ToggleGodmode()
{
	godmodeEnabled = !godmodeEnabled;
	WaitForKeyRelease(VK_NUMPAD3);
	system("cls");
	showMenu(infiniteAmmoEnabled, noclipEnabled, godmodeEnabled, rapidFireEnabled, noRecoilEnabled, noKickbackEnabled, noKickbackLocked, invisEnabled);
}

void MaintainGodmode()
{
	int* health = reinterpret_cast<int*>(playerBase + offset::health);
	int* armor = reinterpret_cast<int*>(playerBase + offset::armor);

	while (godmodeEnabled && !GetAsyncKeyState(VK_NUMPAD0))
	{
		*health = 999;
		*armor = 999;
		Sleep(5);

		if (GetAsyncKeyState(VK_NUMPAD3) & 1)
		{
			godmodeEnabled = false;
			*health = 100;
			*armor = 100;
			WaitForKeyRelease(VK_NUMPAD3);
			system("cls");
			showMenu(infiniteAmmoEnabled, noclipEnabled, godmodeEnabled, rapidFireEnabled, noRecoilEnabled, noKickbackEnabled, noKickbackLocked, invisEnabled);
			break;
		}
	}
}

void ToggleRapidFire()
{
	rapidFireEnabled = !rapidFireEnabled;
	WaitForKeyRelease(VK_NUMPAD4);
	system("cls");
	showMenu(infiniteAmmoEnabled, noclipEnabled, godmodeEnabled, rapidFireEnabled, noRecoilEnabled, noKickbackEnabled, noKickbackLocked, invisEnabled);
}

void MaintainRapidFire()
{
	while (rapidFireEnabled && !GetAsyncKeyState(VK_NUMPAD0))
	{
		// current weapon so these pointers change
		uintptr_t currentWeapon = *reinterpret_cast<uintptr_t*>(playerBase + 0x368);
		uintptr_t currentWeaponStatsStatic = *reinterpret_cast<uintptr_t*>(currentWeapon + 0xC);
		uintptr_t currentWeaponStatsDynamic = *reinterpret_cast<uintptr_t*>(currentWeapon + 0x10);

		int32_t* pistolDelay = reinterpret_cast<int32_t*>((uintptr_t)playerBase + offset::pistolDelay);
		int32_t* carbineDelay = reinterpret_cast<int32_t*>((uintptr_t)playerBase + offset::carbineDelay);
		int32_t* shotgunDelay = reinterpret_cast<int32_t*>((uintptr_t)playerBase + offset::shotgunDelay);
		int32_t* subgunDelay = reinterpret_cast<int32_t*>((uintptr_t)playerBase + offset::subgunDelay);
		int32_t* sniperDelay = reinterpret_cast<int32_t*>((uintptr_t)playerBase + offset::sniperDelay);
		int32_t* arDelay = reinterpret_cast<int32_t*>((uintptr_t)playerBase + offset::arDelay);
		int32_t* grenadeDelay = reinterpret_cast<int32_t*>((uintptr_t)playerBase + offset::grenadeDelay);
		int32_t* akimboDelay = reinterpret_cast<int32_t*>((uintptr_t)playerBase + offset::akimboDelay);
		BOOL* automaticWeapon = reinterpret_cast<BOOL*>((uintptr_t)currentWeaponStatsStatic + offset::automaticWeapon);

		*pistolDelay = 0;
		*carbineDelay = 0;
		*shotgunDelay = 0;
		*subgunDelay = 0;
		*sniperDelay = 0;
		*arDelay = 0;
		*grenadeDelay = 0;
		*akimboDelay = 0;
		*automaticWeapon = TRUE;
		Sleep(5);

		if (GetAsyncKeyState(VK_NUMPAD4) & 1)
		{
			rapidFireEnabled = false;
			WaitForKeyRelease(VK_NUMPAD4);
			system("cls");
			showMenu(infiniteAmmoEnabled, noclipEnabled, godmodeEnabled, rapidFireEnabled, noRecoilEnabled, noKickbackEnabled, noKickbackLocked, invisEnabled);
			break;
		}
	}
}