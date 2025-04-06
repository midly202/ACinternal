#include <Windows.h>
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