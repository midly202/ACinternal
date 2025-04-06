#include <iostream>
#include <Psapi.h>
#include <Windows.h>
#include "offsets.h"

extern bool godmodeEnabled;
extern bool infiniteAmmoEnabled;
extern bool noclipEnabled;
extern bool rapidFireEnabled;
extern uintptr_t playerBase;

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

bool CanUninject(bool thread1Running, bool thread2Running, bool thread3running)
{
	if (thread1Running || thread2Running || thread3running)
		return false; // Returns false if either thread is still running
	else
		return true; // Returns true if both threads have exited
}

void showMenu(bool infiniteAmmo, bool noClip, bool Godmode, bool rapidFireEnabled)
{
	std::cout << "[1] Infinite Ammo [" << (infiniteAmmo ? "ON" : "OFF") << "]\n";
	std::cout << "[2] NoClip [" << (noClip ? "ON" : "OFF") << "]\n";
	std::cout << "[3] Godmode [" << (Godmode ? "ON" : "OFF") << "]\n";
	std::cout << "[4] Rapid Fire [" << (rapidFireEnabled ? "ON" : "OFF") << "]\n";
}

void WaitForKeyRelease(int vkKey)
{
	while (GetAsyncKeyState(vkKey)) Sleep(10);
}

void ToggleGodmode()
{
	godmodeEnabled = !godmodeEnabled;
	WaitForKeyRelease(VK_NUMPAD3);
	system("cls");
	showMenu(infiniteAmmoEnabled, noclipEnabled, godmodeEnabled, rapidFireEnabled);
}

void MaintainGodmode()
{
	int* health = reinterpret_cast<int*>(playerBase + offset::health);
	int* armor = reinterpret_cast<int*>(playerBase + offset::armor);

	while (godmodeEnabled && !GetAsyncKeyState(VK_NUMPAD0))
	{
		*health = 100;
		*armor = 100;
		Sleep(5);

		if (GetAsyncKeyState(VK_NUMPAD3) & 1)
		{
			godmodeEnabled = false;
			WaitForKeyRelease(VK_NUMPAD3);
			system("cls");
			showMenu(infiniteAmmoEnabled, noclipEnabled, godmodeEnabled, rapidFireEnabled);
			break;
		}
	}
}

void ToggleRapidFire()
{
	rapidFireEnabled = !rapidFireEnabled;
	WaitForKeyRelease(VK_NUMPAD4);
	system("cls");
	showMenu(infiniteAmmoEnabled, noclipEnabled, godmodeEnabled, rapidFireEnabled);
}

void MaintainRapidFire()
{
	int32_t* pistolDelay = reinterpret_cast<int32_t*>((uintptr_t)playerBase + offset::pistolDelay);
	int32_t* arDelay = reinterpret_cast<int32_t*>((uintptr_t)playerBase + offset::arDelay);

	while (rapidFireEnabled && !GetAsyncKeyState(VK_NUMPAD0))
	{
		*pistolDelay = 0;
		*arDelay = 0;
		Sleep(5);

		if (GetAsyncKeyState(VK_NUMPAD4) & 1)
		{
			rapidFireEnabled = false;
			WaitForKeyRelease(VK_NUMPAD4);
			system("cls");
			showMenu(infiniteAmmoEnabled, noclipEnabled, godmodeEnabled, rapidFireEnabled);
			break;
		}
	}
}