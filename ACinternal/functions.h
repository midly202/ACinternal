#include <iostream>
#include <Psapi.h>
#include <Windows.h>
#include <vector>
#include <cstdint>
#include <math.h>
#include <thread>
#include "offsets.h"

#define M_PI 3.14159265358979323846

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
extern bool aimbotEnabled;
extern uintptr_t baseAddress;
extern uintptr_t playerBase;
extern uintptr_t entityList;
// extern uintptr_t currentWeapon;
extern uintptr_t ammoFunc;
extern ent* player;
extern gameInfo* game;
extern currWeapon* currentWeapon;
extern weaponStatsDynamic* currentWeaponStatsDynamic;
extern weaponStatsStatic* currentWeaponStatsStatic;
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

bool CanUninject(bool thread1Running, bool thread2Running, bool thread3running, bool thread4Running, bool thread5Running)
{
	if (thread1Running || thread2Running || thread3running || thread4Running || thread5Running)
		return false; // Returns false if a thread is still running
	else
		return true; // Returns true if all threads have exited
}

void showMenu(bool infiniteAmmo, bool noClip, bool Godmode, bool rapidFireEnabled, bool noRecoilEnabled, bool noKickbackEnabled, bool noKickbackLocked, bool invisEnabled, bool aimbotEnabled)
{
	std::cout << "[1] Infinite Ammo [" << (infiniteAmmo ? "ON" : "OFF") << "]\n";
	std::cout << "[2] NoClip [" << (noClip ? "ON" : "OFF") << "]\n";
	std::cout << "[3] Godmode [" << (Godmode ? "ON" : "OFF") << "]\n";
	std::cout << "[4] Rapid Fire [" << (rapidFireEnabled ? "ON" : "OFF") << "]\n";
	std::cout << "[5] No Recoil [" << (noRecoilEnabled ? "ON" : "OFF") << "]\n";
	std::cout << "[6] No Kickback [" << (noKickbackEnabled ? "ON" : "OFF") << (noKickbackLocked ? "] [LOCKED]" : "]") << "\n";
	std::cout << "[7] Invisible [" << (invisEnabled ? "ON" : "OFF") << "]\n";
	std::cout << "[8] Aimbot [" << (aimbotEnabled ? "ON" : "OFF") << "]\n";
}

void WaitForKeyRelease(int vkKey)
{
	while (GetAsyncKeyState(vkKey)) Sleep(10);
}

float euclidean_distance(float x, float y) 
{
	return sqrtf((x * x) + (y * y));
}

struct ViewMatrix {
	float Matrix[16]{};
};

struct Vec2 {
	float x, y;
};

struct Vec3 {
	float x, y, z;
};

struct Vec4 {
	float x, y, z, w;
};

struct Angle
{
	float Yaw, Pitch;
};

bool WorldToScreen(Vec3 pos, Vec2& screen, float matrix[16], int windowWidth, int windowHeight)
{
	Vec4 clipCoords = {};

	clipCoords.x = pos.x * matrix[0] + pos.y * matrix[4] + pos.z * matrix[8] + matrix[12];
	clipCoords.y = pos.x * matrix[1] + pos.y * matrix[5] + pos.z * matrix[9] + matrix[13];
	clipCoords.z = pos.x * matrix[2] + pos.y * matrix[6] + pos.z * matrix[10] + matrix[14];
	clipCoords.w = pos.x * matrix[3] + pos.y * matrix[7] + pos.z * matrix[11] + matrix[15];

	if (clipCoords.w < 0.1f)
	{
		return false;
	}

	Vec3 TranslatedCoords;
	TranslatedCoords.x = clipCoords.x / clipCoords.w;
	TranslatedCoords.y = clipCoords.y / clipCoords.w;
	TranslatedCoords.z = clipCoords.z / clipCoords.w;

	screen.x = (windowWidth / 2 * TranslatedCoords.x) + (TranslatedCoords.x + windowWidth / 2);
	screen.y = -(windowHeight / 2 * TranslatedCoords.y) + (TranslatedCoords.y + windowHeight / 2);

	return true;
}

void ToggleNoKickback()
{
	noKickbackEnabled = true;
	noKickbackLocked = true;
	WaitForKeyRelease(VK_NUMPAD6);
	system("cls");
	showMenu(infiniteAmmoEnabled, noclipEnabled, godmodeEnabled, rapidFireEnabled, noRecoilEnabled, noKickbackEnabled, noKickbackLocked, invisEnabled, aimbotEnabled);
}

void MaintainNoKickback()
{
	while (noKickbackEnabled && !GetAsyncKeyState(VK_NUMPAD0))
	{
		// current weapon so this pointers changes
		DWORD* playerBasePtr = (DWORD*)(baseAddress + offset::playerBase);
		if (!playerBasePtr || !*playerBasePtr) continue;

		ent* player = (ent*)(*playerBasePtr);

		uintptr_t currentWeaponAddr = *(uintptr_t*)((uintptr_t)player + 0x368);
		if (!currentWeaponAddr) continue;

		currWeapon* currentWeapon = (currWeapon*)currentWeaponAddr;

		uintptr_t currentWeaponStatsStaticAddr = *(uintptr_t*)(currentWeaponAddr + 0xC);
		if (!currentWeaponStatsStaticAddr) continue;

		weaponStatsStatic* currentWeaponStatsStatic = (weaponStatsStatic*)currentWeaponStatsStaticAddr;

		currentWeaponStatsStatic->weaponKickback = 0;

		Sleep(5);
	}
}

void ToggleGodmode()
{
	godmodeEnabled = !godmodeEnabled;
	WaitForKeyRelease(VK_NUMPAD3);
	system("cls");
	showMenu(infiniteAmmoEnabled, noclipEnabled, godmodeEnabled, rapidFireEnabled, noRecoilEnabled, noKickbackEnabled, noKickbackLocked, invisEnabled, aimbotEnabled);
}

void MaintainGodmode()
{
	while (godmodeEnabled && !GetAsyncKeyState(VK_NUMPAD0))
	{
		DWORD* playerBasePtr = (DWORD*)(baseAddress + offset::playerBase);
		if (!playerBasePtr || !*playerBasePtr) continue;

		ent* player = (ent*)(*playerBasePtr);

		player->health = 999;
		player->armor = 999;
		Sleep(5);

		if (GetAsyncKeyState(VK_NUMPAD3) & 1)
		{
			godmodeEnabled = false;
			player->health = 100;
			player->armor = 100;
			WaitForKeyRelease(VK_NUMPAD3);
			system("cls");
			showMenu(infiniteAmmoEnabled, noclipEnabled, godmodeEnabled, rapidFireEnabled, noRecoilEnabled, noKickbackEnabled, noKickbackLocked, invisEnabled, aimbotEnabled);
			break;
		}
	}
}

void ToggleRapidFire()
{
	rapidFireEnabled = !rapidFireEnabled;
	WaitForKeyRelease(VK_NUMPAD4);
	system("cls");
	showMenu(infiniteAmmoEnabled, noclipEnabled, godmodeEnabled, rapidFireEnabled, noRecoilEnabled, noKickbackEnabled, noKickbackLocked, invisEnabled, aimbotEnabled);
}

void MaintainRapidFire()
{
	while (rapidFireEnabled && !GetAsyncKeyState(VK_NUMPAD0))
	{
		/* ————————————————————————————
		// old pointer-style access (fallback/backup)

		uintptr_t currentWeapon = *reinterpret_cast<uintptr_t*>(playerBase + 0x368);
		uintptr_t currentWeaponStatsStatic = *reinterpret_cast<uintptr_t*>(currentWeapon + 0xC);

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
		———————————————————————————— */

		DWORD* playerBasePtr = (DWORD*)(baseAddress + offset::playerBase);
		if (!playerBasePtr || !*playerBasePtr) continue;

		ent* player = (ent*)(*playerBasePtr);

		uintptr_t currentWeaponAddr = *(uintptr_t*)((uintptr_t)player + 0x368);
		if (!currentWeaponAddr) continue;

		currWeapon* currentWeapon = (currWeapon*)currentWeaponAddr;

		uintptr_t currentWeaponStatsStaticAddr = *(uintptr_t*)(currentWeaponAddr + 0xC);
		if (!currentWeaponStatsStaticAddr) continue;

		weaponStatsStatic* currentWeaponStatsStatic = (weaponStatsStatic*)currentWeaponStatsStaticAddr;

		player->pistolDelay = 0;
		player->carbineDelay = 0;
		player->shotgunDelay = 0;
		player->subgunDelay = 0;
		player->sniperDelay = 0;
		player->arDelay = 0;
		player->grenadeDelay = 0;
		player->akimboDelay = 0;

		currentWeaponStatsStatic->automaticWeapon = TRUE;

		Sleep(5);

		if (GetAsyncKeyState(VK_NUMPAD4) & 1)
		{
			rapidFireEnabled = false;
			WaitForKeyRelease(VK_NUMPAD4);
			system("cls");
			showMenu(infiniteAmmoEnabled, noclipEnabled, godmodeEnabled, rapidFireEnabled,
				noRecoilEnabled, noKickbackEnabled, noKickbackLocked, invisEnabled, aimbotEnabled);
			break;
		}
	}
}

void ToggleAimbot()
{
	aimbotEnabled = !aimbotEnabled;
	WaitForKeyRelease(VK_NUMPAD8);
	system("cls");
	showMenu(infiniteAmmoEnabled, noclipEnabled, godmodeEnabled, rapidFireEnabled, noRecoilEnabled, noKickbackEnabled, noKickbackLocked, invisEnabled, aimbotEnabled);
}

void MaintainAimbot()
{
	while (aimbotEnabled && !GetAsyncKeyState(VK_NUMPAD0))
	{
		// Get local player
		ent* player = *(ent**)(baseAddress + offset::playerBase);
		if (!player) continue;

		// Get entity list
		ent** entityList = *(ent***)(0x58AC04);
		if (!entityList) continue;

		// Get current player count
		int* current_players = (int*)(0x58AC0C);
		if (!current_players || *current_players <= 0 || *current_players > 32) continue;

		float closest_player = -1.0f;
		float closest_yaw = 0.0f;
		float closest_pitch = 0.0f;

		for (int i = 0; i < *current_players; i++) {
			ent* enemy = entityList[i];
			if (!enemy || enemy == player || !enemy->alive) continue;

			// Calculate positional difference
			float abspos_x = enemy->posHead.x - player->posHead.x;
			float abspos_y = enemy->posHead.y - player->posHead.y;
			float abspos_z = enemy->posHead.z - player->posHead.z;

			float temp_distance = euclidean_distance(abspos_x, abspos_y);
			if (closest_player == -1.0f || temp_distance < closest_player) {
				closest_player = temp_distance;

				float yaw = atan2f(abspos_y, abspos_x) * (180.0f / M_PI);
				closest_yaw = yaw + 90.0f;

				// Fixing low angle jitter
				if (abspos_y < 0) abspos_y *= -1;
				if (abspos_y < 5) {
					if (abspos_x < 0) abspos_x *= -1;
					abspos_y = abspos_x;
				}
				float horizontalDistance = sqrtf(abspos_x * abspos_x + abspos_y * abspos_y);
				float pitch = atan2f(abspos_z, horizontalDistance) * (180.0f / M_PI);
				closest_pitch = pitch;
			}
		}

		// Apply view angles
		player->angles.x = closest_yaw;
		player->angles.y = closest_pitch;

		Sleep(1);

		// Toggle off
		if (GetAsyncKeyState(VK_NUMPAD8) & 1)
		{
			aimbotEnabled = false;
			WaitForKeyRelease(VK_NUMPAD8);
			system("cls");
			showMenu(infiniteAmmoEnabled, noclipEnabled, godmodeEnabled, rapidFireEnabled, noRecoilEnabled, noKickbackEnabled, noKickbackLocked, invisEnabled, aimbotEnabled);
			break;
		}
	}
}