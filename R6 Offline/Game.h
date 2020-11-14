#pragma once
#include "Memory.h"
#include "Offsets.h"
#include "Vector.h"
#include "Defs.h"

class CGame
{
public:

	uint64_t DecryptEntityInfo(uint64_t entity)
	{
		uint64_t info = Memory.Read<uint64_t>(entity + 0x50);
		info ^= 0x59089C3C53641909;
		info += 0x3E48F409BF6F5A50;
		info ^= 0x155D2A1FBD952158;

		return info;
	}

	uint64_t DecryptEventManager(uint64_t entity)
	{
		uint64_t eventManager = Memory.Read<uint64_t>(entity + 0x30);
		eventManager ^= 0xABD61E6916FBC7F2;
		eventManager += 0x3894F6F1D31B186C;
		eventManager ^= 0xC9956FF9443397DE;

		return eventManager;
	}

	uint64_t DecryptWeaponInfo(uint64_t entity)
	{
		uint64_t weaponInfo = Memory.Read<uint64_t>(entity + 0x90);
		weaponInfo = Memory.Read<uint64_t>(weaponInfo + 0x70);
		weaponInfo = Memory.Read<uint64_t>(weaponInfo + 0x288);
		weaponInfo = __ROL8__(weaponInfo, 0x3E);
		weaponInfo -= 0x68;
		weaponInfo = __ROL8__(weaponInfo, 0x15);

		return weaponInfo;
	}

	uint64_t GetLocalPlayer()
	{
		uint64_t localPlayer = Memory.Read<uint64_t>(Memory.base + OFFSET_PROFILE_MANAGER);
		localPlayer = Memory.Read<uint64_t>(localPlayer + 0x88);
		localPlayer = Memory.Read<uint64_t>(localPlayer + 0x0);
		localPlayer = Memory.Read<uint64_t>(localPlayer + 0x30);
		localPlayer -= 0x77;
		localPlayer = __ROL8__(localPlayer, 0x6);
		localPlayer -= 0x59;

		return localPlayer;
	}

	BYTE GetGameState()
	{
		uint64_t stateAddr = Memory.Read<uint64_t>(Memory.base + OFFSET_ROUND_MANAGER);
		BYTE state = Memory.Read<BYTE>(stateAddr + 0x300);
		return state;
	}

	int GetAnimation()
	{
		uint64_t animations = Memory.Read<uint64_t>(Memory.base + 0x5E06DC8); //48 8B 05 ? ? ? ? 48 8B 00 48 8B 00
		animations = Memory.Read<uint64_t>(animations + 0x0);
		animations = Memory.Read<uint64_t>(animations + 0x28);
		animations = Memory.Read<uint64_t>(animations + 0x0);
		animations = Memory.Read<uint64_t>(animations + 0xD8);
		animations = Memory.Read<uint64_t>(animations + 0x18);
		animations = Memory.Read<uint64_t>(animations + 0xC0);

		return Memory.Read<int>(animations + 0xB94);
	}

	uint64_t GetEntityList()
	{
		uint64_t gameManager = Memory.Read<uint64_t>(Memory.base + OFFSET_GAME_MANAGER);
		uint64_t entityList = Memory.Read<uint64_t>(gameManager + 0xE0);
		uint64_t entityList1 = entityList >> 0xC;
		uint64_t entityList2 = entityList << 0x34;
		entityList = entityList1 | entityList2;
		entityList += 0xFDF84BE05A7526D4;
		entityList ^= 0xCA90740F16A90A3C;

		return entityList;
	}

	int GetEntityCount()
	{
		uint64_t gameManager = Memory.Read<uint64_t>(Memory.base + OFFSET_GAME_MANAGER);
		uint64_t entityCount = Memory.Read<uint64_t>(gameManager + 0xE8);
		uint64_t entityCount1 = entityCount >> 0xC;
		uint64_t entityCount2 = entityCount << 0x34;
		entityCount = entityCount1 | entityCount2;
		entityCount += 0xFDF84BE05A7526D4;
		entityCount ^= 0xCA90740F16A90A3C;
		int count = (int)(entityCount ^ 0x18C0000000);
		return count;
	}

	uint64_t GetEntityByID(int id)
	{
		uint64_t entityList = GetEntityList();
		if (!entityList) return 0;

		uint64_t entity = Memory.Read<uint64_t>(entityList + (sizeof(PVOID) * id));
		if (!entity) return 0;

		return entity;
	}

	BYTE EntityTeam(uint64_t entity)
	{
		//Attack  3
		//Defense 4

		uint64_t teamInfo = Memory.Read<uint64_t>(entity + 0xD0);
		teamInfo = Memory.Read<uint64_t>(teamInfo + 0x98);
		teamInfo ^= 0x2C27FF3EF7F266E7;
		teamInfo += 0xB6E05CEDB1C858E7;
		teamInfo = __ROL8__(teamInfo, 0x15);
		BYTE team = Memory.Read<BYTE>(teamInfo + 0x30);
		return team;
	}

	void SetEntityTeam(uint64_t entity, BYTE team)
	{
		//Attack  3
		//Defense 4

		uint64_t teamInfo = Memory.Read<uint64_t>(entity + 0xD0);
		teamInfo = Memory.Read<uint64_t>(teamInfo + 0x98);
		teamInfo = __ROL8__(teamInfo, 0x33);
		teamInfo += 0xDE627E1C01E6C58F;
		teamInfo = __ROL8__(teamInfo, 0x2B);
		Memory.Write<BYTE>(teamInfo + 0x30, team);
	}

	int GetEntityHealth(uint64_t entity)
	{
		uint64_t healthAddr = DecryptEntityInfo(entity);
		healthAddr = Memory.Read<uint64_t>(healthAddr + 0x18);
		healthAddr = Memory.Read<uint64_t>(healthAddr + 0xD8);
		healthAddr = Memory.Read<uint64_t>(healthAddr + 0x8);
		int health = Memory.Read<int>(healthAddr + 0x170);
		return health;
	}

	Vector3 GetEntityPos(uint64_t entity)
	{
		uint64_t posAddr = DecryptEntityInfo(entity);
		posAddr = Memory.Read<uint64_t>(posAddr + 0x18);
		Vector3 pos = Memory.Read<Vector3>(posAddr + 0x50);
		return pos;
	}

	Vector3 GetEntityHead(uint64_t entity)
	{
		uint64_t skeleton = DecryptEntityInfo(entity);
		Vector3 head = Memory.Read<Vector3>(skeleton + 0x1070);
		return head;
	}

	bool IsEnemy(uint64_t entity)
	{
		if (entity == 0) return false;

		if (GetLocalPlayer() == entity) return false;

		//if (entity_team(get_localplayer()) == entity_team(entity)) return false; //Comment out if you want this to work for teamates/NPC's

		return true;
	}

	uint64_t GetGameCamera()
	{
		uint64_t camera = Memory.Read<uint64_t>(Memory.base + OFFSET_PROFILE_MANAGER);
		camera = Memory.Read<uint64_t>(camera + 0x88);
		camera = Memory.Read<uint64_t>(camera + 0x0);
		camera = Memory.Read<uint64_t>(camera + 0x220);
		camera = Memory.Read<uint64_t>(camera + 0x410);
		return camera;
	}

	Vector3 GetViewRight()
	{
		Vector3 right = Memory.Read<Vector3>(GetGameCamera() + 0x7A0);
		return right;
	}

	Vector3 GetViewUp()
	{
		Vector3 up = Memory.Read<Vector3>(GetGameCamera() + 0x7B0);
		return up;
	}

	Vector3 GetViewForward()
	{
		Vector3 forward = Memory.Read<Vector3>(GetGameCamera() + 0x7C0);
		return forward;
	}

	Vector3 GetViewTranslation()
	{
		Vector3 translation = Memory.Read<Vector3>(GetGameCamera() + 0x7D0);
		return translation;
	}

	float GetViewFovX()
	{
		float x = Memory.Read<float>(GetGameCamera() + 0x7E0);
		return x;
	}

	float GetViewFovY()
	{
		float y = Memory.Read<float>(GetGameCamera() + 0x7F4);
		return y;
	}

	bool WorldToScreen(Vector3 position, Vector2& Screen)
	{
		Vector3 temp = position - GetViewTranslation();
		float x = temp.Dot(GetViewRight());
		float y = temp.Dot(GetViewUp());
		float z = temp.Dot(GetViewForward() * -1.f);

		int width = GetSystemMetrics(SM_CXSCREEN);
		int height = GetSystemMetrics(SM_CYSCREEN);

		Screen.x = (width / 2.f) * (1.f + x / GetViewFovX() / z);
		Screen.y = (height / 2.f) * (1.f - y / GetViewFovY() / z);

		return z >= 1.0f ? true : false;
	}
};

inline CGame Game;