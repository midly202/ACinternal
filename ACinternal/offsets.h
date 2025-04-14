#include <Windows.h>
#include <vector>

namespace offset
{
	constexpr uintptr_t playerBase = 0x17E0A8;
	constexpr uintptr_t gameInfo = 0x18AC04;
	constexpr uintptr_t entityList = 0x18AC04; // dereference gameInfo + 0 offset to get to entitylist

	// ent
	constexpr uintptr_t posHead = 0x4;
	constexpr uintptr_t directionMoving = 0x10;
	constexpr uintptr_t velocity = 0x1C;
	constexpr uintptr_t posBody = 0x28;
	constexpr uintptr_t viewAngles = 0x34;
	constexpr uintptr_t airtime = 0x48;
	constexpr uintptr_t touchingGround = 0x5D;
	constexpr uintptr_t forceJump = 0x5F;
	constexpr uintptr_t crouching = 0x61;
	constexpr uintptr_t forceCrouch = 0x63;
	constexpr uintptr_t standingStill = 0x65;
	constexpr uintptr_t noClip = 0x76;
	constexpr uintptr_t health = 0xEC;
	constexpr uintptr_t armor = 0xF0;
	constexpr uintptr_t nextGunId = 0xF8;
	constexpr uintptr_t alive = 0x104;
	constexpr uintptr_t pistolAmmo = 0x108;
	constexpr uintptr_t carbineAmmo = 0x10C;
	constexpr uintptr_t shotgunAmmo = 0x110;
	constexpr uintptr_t subgunAmmo = 0x114;
	constexpr uintptr_t sniperAmmo = 0x118;
	constexpr uintptr_t arAmmo = 0x11C;
	constexpr uintptr_t akimboAmmo = 0x124;
	constexpr uintptr_t pistolClip = 0x12C;
	constexpr uintptr_t carbineClip = 0x12C;
	constexpr uintptr_t shotgunClip = 0x12C;
	constexpr uintptr_t subgunClip = 0x12C;
	constexpr uintptr_t sniperClip = 0x12C;
	constexpr uintptr_t arClip = 0x140;
	constexpr uintptr_t grenadeClip = 0x144;
	constexpr uintptr_t akimboClip = 0x148;
	constexpr uintptr_t isKnifing = 0x14C;
	constexpr uintptr_t pistolDelay = 0x150;
	constexpr uintptr_t carbineDelay = 0x154;
	constexpr uintptr_t shotgunDelay = 0x158;
	constexpr uintptr_t subgunDelay = 0x15C;
	constexpr uintptr_t sniperDelay = 0x160;
	constexpr uintptr_t arDelay = 0x164;
	constexpr uintptr_t grenadeDelay = 0x168;
	constexpr uintptr_t akimboDelay = 0x16C;
	constexpr uintptr_t timesKnifed = 0x170;
	constexpr uintptr_t timesShotPistol = 0x174;
	constexpr uintptr_t timesShotCarbine = 0x178;
	constexpr uintptr_t timesShotShotgun = 0x17C;
	constexpr uintptr_t timesShotSubgun = 0x180;
	constexpr uintptr_t timesShotSniper = 0x184;
	constexpr uintptr_t timesShotAr = 0x188;
	constexpr uintptr_t timesThrownNade = 0x18C;
	constexpr uintptr_t timesShotAkimbo = 0x190;
	constexpr uintptr_t forceFire = 0x204;
	constexpr uintptr_t username = 0x205;

	// currWeapon
	constexpr uintptr_t gunId = 0x4;

	// weaponStatsStatic
	constexpr uintptr_t weaponString = 0x0;
	constexpr uintptr_t weaponSubString = 0x17;
	constexpr uintptr_t weaponSoundId = 0x42;
	constexpr uintptr_t weaponReloadSoundId = 0x44;
	constexpr uintptr_t weaponReloadSpeed = 0x46;
	constexpr uintptr_t weaponDelay = 0x48;
	constexpr uintptr_t weaponKickback = 0x54;
	constexpr uintptr_t weaponClipSize = 0x56;
	constexpr uintptr_t weaponRecoil = 0x60;
	constexpr uintptr_t automaticWeapon = 0x66;

	// weaponStatsDynamic
	constexpr uintptr_t weaponAmmo = 0x0;
	constexpr uintptr_t weaponClip = 0x24;
	constexpr uintptr_t weaponDelayDynamic = 0x48;
	constexpr uintptr_t weaponShotsFired = 0x6C;
}

struct Matrix4x4 {
	float Matrix[16]{};
};

struct Vector2 {
	float x, y;
};

struct Vector3 {
	float x, y, z;
};

struct Vector4 {
	float x, y, z, w;
};

// Created with ReClass.NET 1.2 by KN4CK3R

class ent
{
public:
	char pad_0000[4]; //0x0000
	Vector3 posHead; //0x0004
	Vector3 velocity; //0x0010
	Vector3 velocity2; //0x001C
	Vector3 pos; //0x0028
	Vector2 angles; //0x0034
	char pad_003C[12]; //0x003C
	int32_t howLongFalling; //0x0048
	char pad_004C[17]; //0x004C
	bool isTouchingGround; //0x005D
	int8_t jumpCooldown; //0x005E
	bool forceJump; //0x005F
	char pad_0060[1]; //0x0060
	bool isCrouching; //0x0061
	char pad_0062[1]; //0x0062
	bool forceCrouch; //0x0063
	char pad_0064[1]; //0x0064
	bool isStandingStill; //0x0065
	bool isAiming; //0x0066
	char pad_0067[15]; //0x0067
	int16_t noClip; //0x0076
	char pad_0078[8]; //0x0078
	int8_t moveLeft; //0x0080
	int8_t moveRight; //0x0081
	int8_t moveForward; //0x0082
	int8_t moveBackward; //0x0083
	char pad_0084[104]; //0x0084
	int32_t health; //0x00EC
	int32_t armor; //0x00F0
	int32_t currentGunId; //0x00F4
	int32_t nextGunId; //0x00F8
	int32_t currentGunId2; //0x00FC
	bool hasAkimbo; //0x0100
	char pad_0101[3]; //0x0101
	bool alive; //0x0104
	char pad_0105[3]; //0x0105
	int32_t pistolAmmo; //0x0108
	int32_t carbineAmmo; //0x010C
	int32_t shotgunAmmo; //0x0110
	int32_t subgunAmmo; //0x0114
	int32_t sniperAmmo; //0x0118
	int32_t arAmmo; //0x011C
	char pad_0120[4]; //0x0120
	int32_t akimboAmmo; //0x0124
	int32_t knifeClip; //0x0128
	int32_t pistolClip; //0x012C
	int32_t carbineClip; //0x0130
	int32_t shotgunClip; //0x0134
	int32_t subgunClip; //0x0138
	int32_t sniperClip; //0x013C
	int32_t arClip; //0x0140
	int32_t grenadeClip; //0x0144
	int32_t akimboClip; //0x0148
	bool knifing; //0x014C
	bool knifing2; //0x014D
	char pad_014E[2]; //0x014E
	int32_t pistolDelay; //0x0150
	int32_t carbineDelay; //0x0154
	int32_t shotgunDelay; //0x0158
	int32_t subgunDelay; //0x015C
	int32_t sniperDelay; //0x0160
	int32_t arDelay; //0x0164
	int32_t grenadeDelay; //0x0168
	int32_t akimboDelay; //0x016C
	int32_t timesKnifed; //0x0170
	int32_t timesShotPistol; //0x0174
	int32_t timesShotCarbine; //0x0178
	int32_t timesShotShotgun; //0x017C
	int32_t timesShotSubgun; //0x0180
	int32_t timesShotSniper; //0x0184
	int32_t timesShotAr; //0x0188
	int32_t timesThrownNade; //0x018C
	int32_t timesShotAkimbo; //0x0190
	char pad_0194[112]; //0x0194
	bool forceFire; //0x0204
	char username[16]; //0x0205
	char pad_0215[247]; //0x0215
	int8_t team; //0x030C
	char pad_030D[91]; //0x030D
	class currWeapon* currWeaponPtr; //0x0368
	char pad_036C[2048]; //0x036C
}; //Size: 0x0B6C

class currWeapon
{
public:
	char pad_0000[4]; //0x0000
	int8_t weaponId; //0x0004
	char pad_0005[3]; //0x0005
	class ent* weaponOwner; //0x0008
	class weaponStatsStatic* weaponStatsStatic; //0x000C
	class weaponStatsDynamic* weaponStatsDynamic; //0x0010
	char pad_0014[20]; //0x0014
	bool isAiming; //0x0028
	char pad_0029[1003]; //0x0029
}; //Size: 0x0414

class weaponStatsDynamic
{
public:
	int32_t currWeaponAmmo; //0x0000
	char pad_0004[32]; //0x0004
	int32_t currWeaponClip; //0x0024
	char pad_0028[32]; //0x0028
	int32_t currWeaponDelay; //0x0048
	char pad_004C[32]; //0x004C
	int32_t shotsFired; //0x006C
}; //Size: 0x0070

class weaponStatsStatic
{
public:
	char modelName[16]; //0x0000
	char pad_0010[7]; //0x0010
	char weaponName[16]; //0x0017
	char pad_0027[27]; //0x0027
	int16_t soundId; //0x0042
	int16_t reloadSoundId; //0x0044
	int8_t reloadAnimationSpeed; //0x0046
	int8_t reloadSpeed; //0x0047
	int16_t weaponDelay; //0x0048
	int16_t maxDamagePerShot; //0x004A
	int16_t armorPen; //0x004C
	char pad_004E[4]; //0x004E
	int16_t spread; //0x0052
	int16_t weaponKickback; //0x0054
	int16_t weaponClipSize; //0x0056
	char pad_0058[2]; //0x0058
	int8_t weaponMovement; //0x005A
	char pad_005B[3]; //0x005B
	int8_t weaponRecoil1; //0x005E
	char pad_005F[1]; //0x005F
	int16_t weaponRecoil2; //0x0060
	char pad_0062[4]; //0x0062
	bool automaticWeapon; //0x0066
	char pad_0067[1]; //0x0067
}; //Size: 0x0068

class camera
{
public:
	Matrix4x4 viewMatrix; //0x0000
	char pad_0040[1428]; //0x0040
}; //Size: 0x05D4

class gameInfo
{
public:
	class entityList* N00000DBC; //0x0000
	char pad_0004[4]; //0x0004
	int8_t currentPlayers; //0x0008
	char pad_0009[2103]; //0x0009
}; //Size: 0x0840

class entityList
{
public:
	char pad_0000[1048]; //0x0000
}; //Size: 0x0418