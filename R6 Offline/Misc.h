#pragma once
#include "Game.h"

void UnlockAll()
{
	//4C 8B 38 48 85 F6
	//Patternscan and then break and trace in cheat engine for R15 or dereference RAX
	//Then pattern scan for one of the above addresses
	//Dont read the last pointer but instead add it
	//Then adds 0xC to get to the owned bool
	//After every season update the loop number to the number of operators

	uint64_t contentManager = Memory.Read<uint64_t>(Memory.base + 0x5DE5C70);
	contentManager = Memory.Read<uint64_t>(contentManager + 0x218);
	contentManager = Memory.Read<uint64_t>(contentManager);
	contentManager += 0x30C;

	for (int i = 0; i < 57; i++)
	{
		Memory.Write<uint32_t>(contentManager + ((uint64_t)i * 0x10), true);
	}
}

void RunAndGun(bool enable)
{
	//Go to R&S1 patch offset and pattern scan for the register address that is added with 0x178 and use that

	static bool getOrigRCX = true;
	static uint64_t origRCX = 0x0;
	static bool getOrigRCX40 = true;
	static uint64_t origRCX40 = 0x0;

	if (enable)
	{
		uint64_t rcx = Memory.Read<uint64_t>(Memory.base + 0x68E5F80);
		rcx = Memory.Read<uint64_t>(rcx + 0x18);
		rcx = Memory.Read<uint64_t>(rcx);
		rcx = Memory.Read<uint64_t>(rcx + 0x30);
		rcx = Memory.Read<uint64_t>(rcx + 0x30);
		rcx = Memory.Read<uint64_t>(rcx + 0x50);
		rcx = Memory.Read<uint64_t>(rcx + 0x8);
		rcx = Memory.Read<uint64_t>(rcx + 0x148);
		uint64_t rcxW = Memory.Read<uint64_t>(rcx + 0x30);
		rcx = Memory.Read<uint64_t>(rcxW + 0x178);
		if (getOrigRCX)
		{
			origRCX = rcx;
			getOrigRCX = false;
		}
		if (getOrigRCX40)
		{
			origRCX40 = Memory.Read<uint64_t>(rcx + 0x40);
			getOrigRCX40 = false;
		}
		Memory.Write<uint64_t>(rcx + 0x40, 0x0);
		Memory.Write<uint8_t>(rcx + 0x80, 0x1);
		Memory.Write<uint64_t>(rcxW + 0x178, 0x0);
	}
	else
	{
		uint64_t rcx = Memory.Read<uint64_t>(Memory.base + 0x68CEC90);
		rcx = Memory.Read<uint64_t>(rcx + 0x18);
		rcx = Memory.Read<uint64_t>(rcx);
		rcx = Memory.Read<uint64_t>(rcx + 0x30);
		rcx = Memory.Read<uint64_t>(rcx + 0x30);
		rcx = Memory.Read<uint64_t>(rcx + 0x50);
		rcx = Memory.Read<uint64_t>(rcx + 0x8);
		rcx = Memory.Read<uint64_t>(rcx + 0x148);
		rcx = Memory.Read<uint64_t>(rcx + 0x30);
		Memory.Write<uint64_t>(rcx + 0x178, origRCX);
		getOrigRCX = true;
		rcx = Memory.Read<uint64_t>(rcx + 0x178);
		Memory.Write<uint64_t>(rcx + 0x40, origRCX40);
		getOrigRCX40 = true;
	}
}

void Speed(bool enable, int speed)
{
	uint64_t speedAddr = Game.DecryptEventManager(Game.GetLocalPlayer());
	speedAddr = Memory.Read<uint64_t>(speedAddr + 0x30);
	speedAddr = Memory.Read<uint64_t>(speedAddr + 0x38);

	if (Game.GetAnimation() == 1) //Ladder
	{
		Memory.Write<int>(speedAddr + 0x58, enable ? 10000 : 105);
	}
	else if (Game.GetAnimation() == 2) //Rappel
	{
		Memory.Write<int>(speedAddr + 0x58, enable ? 10000 : 105);
	}
	else
	{
		Memory.Write<int>(speedAddr + 0x58, enable ? speed : 105);
	} 
}

void FarKnife(bool enable)
{
	uint64_t knife = Game.DecryptEventManager(Game.GetLocalPlayer());
	knife = Memory.Read<uint64_t>(knife + 0x80);
	knife = Memory.Read<uint64_t>(knife + 0x30);

	Memory.Write<float>(knife + 0x60, enable ? 750 : 1.3f);
	Memory.Write<float>(knife + 0x64, enable ? 750 : 1.3f);
}

void NoRecoil(bool enable)
{
	//Memory.Write<uint8_t>(Memory.base + OFFSET_PATCH_NO_RECOIL + 0x06, enable ? 0x1 : 0x0);

	uint64_t recoil = Game.DecryptWeaponInfo(Game.GetLocalPlayer());
	Memory.Write<bool>(recoil + 0x198, enable ? false : true); //Overwrite
	Memory.Write<float>(recoil + 0x17C, enable ? 0.f : 0.625f); //Horizontal
	Memory.Write<float>(recoil + 0x18C, enable ? 0.f : 0.700f); //Vertical

	//Multiplier
	uint64_t weapon = Memory.Read<uint64_t>(Game.GetLocalPlayer() + 0x90);
	uint64_t curWeapon = Memory.Read<uint64_t>(weapon + 0x70);
	uint64_t arrayToSingle = Memory.Read<uint64_t>(curWeapon + 0x258);
	arrayToSingle = Memory.Read<uint64_t>(arrayToSingle);
	uint64_t curWeaponPreset = Memory.Read<uint64_t>(arrayToSingle + 0x140);
	curWeaponPreset = Memory.Read<uint64_t>(curWeaponPreset);
	Memory.Write<float>(curWeaponPreset + 0x70, enable ? 0.f : -0.2f); //Pull
	Memory.Write<float>(curWeaponPreset + 0x74, enable ? 0.f : 0.325f); //Kick
}

void NoSpread(bool enable)
{
	uint64_t spread = Game.DecryptWeaponInfo(Game.GetLocalPlayer());
	Memory.Write<float>(spread + 0x88, enable ? 0.f : 1.f);
}