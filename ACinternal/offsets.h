#include <Windows.h>
namespace offset
{
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