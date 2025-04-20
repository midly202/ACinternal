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

// ANSI codes
// Standard colors
const std::string RESET = "\033[0m";
const std::string BLACK = "\033[30m";
const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string YELLOW = "\033[33m";
const std::string BLUE = "\033[34m";
const std::string MAGENTA = "\033[35m";
const std::string CYAN = "\033[36m";
const std::string WHITE = "\033[37m";

// Bright colors
const std::string BRIGHT_BLACK = "\033[90m";
const std::string BRIGHT_RED = "\033[91m";
const std::string BRIGHT_GREEN = "\033[92m";
const std::string BRIGHT_YELLOW = "\033[93m";
const std::string BRIGHT_BLUE = "\033[94m";
const std::string BRIGHT_MAGENTA = "\033[95m";
const std::string BRIGHT_CYAN = "\033[96m";
const std::string BRIGHT_WHITE = "\033[97m";

// Backgrounds (standard)
const std::string BG_BLACK = "\033[40m";
const std::string BG_RED = "\033[41m";
const std::string BG_GREEN = "\033[42m";
const std::string BG_YELLOW = "\033[43m";
const std::string BG_BLUE = "\033[44m";
const std::string BG_MAGENTA = "\033[45m";
const std::string BG_CYAN = "\033[46m";
const std::string BG_WHITE = "\033[47m";

// Backgrounds (bright)
const std::string BG_BRIGHT_BLACK = "\033[100m";
const std::string BG_BRIGHT_RED = "\033[101m";
const std::string BG_BRIGHT_GREEN = "\033[102m";
const std::string BG_BRIGHT_YELLOW = "\033[103m";
const std::string BG_BRIGHT_BLUE = "\033[104m";
const std::string BG_BRIGHT_MAGENTA = "\033[105m";
const std::string BG_BRIGHT_CYAN = "\033[106m";
const std::string BG_BRIGHT_WHITE = "\033[107m";

// Text effects
const std::string BOLD = "\033[1m";
const std::string UNDERLINE = "\033[4m";
const std::string BLINK = "\033[5m";
const std::string INVERT = "\033[7m";
const std::string HIDDEN = "\033[8m";

// 256-color mode (text)
const std::string ORANGE_256 = "\033[38;5;208m";
const std::string PINK_256 = "\033[38;5;200m";
const std::string TEAL_256 = "\033[38;5;37m";
const std::string GREY_256 = "\033[38;5;244m";
const std::string PURPLE_256 = "\033[38;5;129m";
const std::string NEON_GREEN_256 = "\033[38;5;118m";
const std::string SKY_BLUE_256 = "\033[38;5;75m";

// RGB True Color (foreground)
const std::string RGB_PURPLE = "\033[38;2;204;153;255m";
const std::string RGB_PINK = "\033[38;2;255;105;180m";
const std::string RGB_ORANGE = "\033[38;2;255;165;0m";
const std::string RGB_SKY = "\033[38;2;135;206;250m";
const std::string RGB_AQUA = "\033[38;2;0;255;255m";
const std::string RGB_LIME = "\033[38;2;50;205;50m";
const std::string RGB_GOLD = "\033[38;2;255;215;0m";
const std::string RGB_CORAL = "\033[38;2;255;127;80m";
const std::string RGB_INDIGO = "\033[38;2;75;0;130m";
const std::string RGB_MINT = "\033[38;2;152;255;152m";

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

void showMenu(bool infiniteAmmo, bool noClip, bool godmode, bool rapidFire, bool noRecoil, bool noKickback, bool invis, bool aimbot)
{
	std::cout << RGB_PURPLE + BOLD << R"(
+-----------------------------------------+
|     /\___/\                             |
|    ( o   o )   Injected!                |
|    (  =^=  )                            |
|    (        )                           |
|    (         )   Let's go.              |
|    (          )))))))))))))))           |
+-----------------------------------------+
)" << RESET << "\n";

	std::cout << BLINK + "[0] Uninject" + RESET << "\n\n";
	std::cout << "[1] [" << (infiniteAmmo ? GREEN + "ON" + RESET : RED + "OFF" + RESET) << "] Infinite Ammo\n";
	std::cout << "[2] [" << (noClip ? GREEN + "ON" + RESET : RED + "OFF" + RESET) << "] NoClip\n";
	std::cout << "[3] [" << (godmode ? GREEN + "ON" + RESET : RED + "OFF" + RESET) << "] Godmode\n";
	std::cout << "[4] [" << (rapidFire ? GREEN + "ON" + RESET : RED + "OFF" + RESET) << "] Rapid Fire\n";
	std::cout << "[5] [" << (noRecoil ? GREEN + "ON" + RESET : RED + "OFF" + RESET) << "] No Recoil\n";
	std::cout << "[6] [" << (noKickback ? GREEN + "ON" + RESET : RED + "OFF" + RESET) << "] No Kickback\n";
	std::cout << "[7] [" << (invis ? GREEN + "ON" + RESET : RED + "OFF" + RESET) << "] Invisible\n";
	std::cout << "[8] [" << (aimbot ? GREEN + "ON" + RESET : RED + "OFF" + RESET) << "] Aimbot\n";
}

void WaitForKeyRelease(int vkKey)
{
	while (GetAsyncKeyState(vkKey)) Sleep(10);
}

bool IsKeyPressedOnce(int vKey)
{
	static bool keyDown[256] = {};
	if (GetAsyncKeyState(vKey) & 0x8000)
	{
		if (!keyDown[vKey])
		{
			keyDown[vKey] = true;
			return true;
		}
	}
	else
	{
		keyDown[vKey] = false;
	}
	return false;
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
	noKickbackEnabled = !noKickbackEnabled;
	WaitForKeyRelease(VK_NUMPAD6);
	system("cls");
	showMenu(infiniteAmmoEnabled, noclipEnabled, godmodeEnabled, rapidFireEnabled, noRecoilEnabled, noKickbackEnabled, invisEnabled, aimbotEnabled);
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

		if (GetAsyncKeyState(VK_NUMPAD6) & 1)
		{
			noKickbackEnabled = false;
			currentWeaponStatsStatic->weaponKickback = 10;
			WaitForKeyRelease(VK_NUMPAD6);
			system("cls");
			showMenu(infiniteAmmoEnabled, noclipEnabled, godmodeEnabled, rapidFireEnabled, noRecoilEnabled, noKickbackEnabled, invisEnabled, aimbotEnabled);
			break;
		}
	}
}

void ToggleGodmode()
{
	godmodeEnabled = !godmodeEnabled;
	WaitForKeyRelease(VK_NUMPAD3);
	system("cls");
	showMenu(infiniteAmmoEnabled, noclipEnabled, godmodeEnabled, rapidFireEnabled, noRecoilEnabled, noKickbackEnabled, invisEnabled, aimbotEnabled);
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
			showMenu(infiniteAmmoEnabled, noclipEnabled, godmodeEnabled, rapidFireEnabled, noRecoilEnabled, noKickbackEnabled, invisEnabled, aimbotEnabled);
			break;
		}
	}
}

void ToggleRapidFire()
{
	rapidFireEnabled = !rapidFireEnabled;
	WaitForKeyRelease(VK_NUMPAD4);
	system("cls");
	showMenu(infiniteAmmoEnabled, noclipEnabled, godmodeEnabled, rapidFireEnabled, noRecoilEnabled, noKickbackEnabled, invisEnabled, aimbotEnabled);
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
			showMenu(infiniteAmmoEnabled, noclipEnabled, godmodeEnabled, rapidFireEnabled, noRecoilEnabled, noKickbackEnabled, invisEnabled, aimbotEnabled);
			break;
		}
	}
}

void ToggleAimbot()
{
	aimbotEnabled = !aimbotEnabled;
	WaitForKeyRelease(VK_NUMPAD8);
	system("cls");
	showMenu(infiniteAmmoEnabled, noclipEnabled, godmodeEnabled, rapidFireEnabled, noRecoilEnabled, noKickbackEnabled, invisEnabled, aimbotEnabled);
}

void MaintainAimbot()
{
	while (aimbotEnabled && !GetAsyncKeyState(VK_NUMPAD0))
	{
		// Toggle off
		if (IsKeyPressedOnce(VK_NUMPAD8))
		{
			aimbotEnabled = false;
			WaitForKeyRelease(VK_NUMPAD8);
			system("cls");
			showMenu(infiniteAmmoEnabled, noclipEnabled, godmodeEnabled, rapidFireEnabled, noRecoilEnabled, noKickbackEnabled, invisEnabled, aimbotEnabled);
			break;
		}

		if (!GetAsyncKeyState(VK_LBUTTON) && !GetAsyncKeyState(VK_RBUTTON)) continue;

		// Get local player
		ent* player = *(ent**)(baseAddress + offset::playerBase);
		if (!player) continue;

		// Get entity list
		ent** entityList = *(ent***)(baseAddress + offset::entityList);
		if (!entityList) continue;

		// Get current player count
		int* current_players = (int*)(0x58AC0C);
		if (!current_players || *current_players <= 0 || *current_players > 32) continue;

		float closest_player = -1.0f;
		float closest_yaw = 0.0f;
		float closest_pitch = 0.0f;

		for (int i = 0; i < *current_players; i++) 
		{
			ent* enemy = entityList[i];
			if (!enemy || enemy == player || enemy->health <= 0 || enemy->team == player->team) continue;

			//IMPORTANT! NEED TO CHANGE!
			// check gamemode if its team game; if so, enemy->team == player->team, compare this flag.

			// Calculate positional difference
			float abspos_x = enemy->posHead.x - player->posHead.x;
			float abspos_y = enemy->posHead.y - player->posHead.y;
			float abspos_z = enemy->posHead.z - player->posHead.z;

			float temp_distance = euclidean_distance(abspos_x, abspos_y);
			if (closest_player == -1.0f || temp_distance < closest_player) 
			{
				closest_player = temp_distance;

				float yaw = atan2f(abspos_y, abspos_x) * (180.0f / M_PI);
				closest_yaw = yaw + 90.0f;

				float horizontalDistance = sqrtf(abspos_x * abspos_x + abspos_y * abspos_y);
				if (horizontalDistance < 0.0001f) horizontalDistance = 0.01f; // avoid division by zero
				float pitch = atan2f(abspos_z, horizontalDistance) * (180.0f / M_PI);
				closest_pitch = pitch;
			}
		}

		if (closest_player != -1.0f)
		{
			player->angles.x = closest_yaw;
			player->angles.y = closest_pitch;
		}

		Sleep(1);
	}
}