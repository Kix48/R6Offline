#pragma once
#include "Game.h"

Vector4 CalculateQuaternion(Vector3 euler)
{
	Vector4 result{};

	auto yaw = (euler.z * 0.01745329251f) * 0.5f;
	auto sy = std::sin(yaw);
	auto cy = std::cos(yaw);

	auto roll = (euler.x * 0.01745329251f) * 0.5f;
	auto sr = std::sin(roll);
	auto cr = std::cos(roll);

	constexpr auto sp = 0.f;
	constexpr auto cp = 1.f;

	result.x = cy * sr * cp - sy * cr * sp;
	result.y = cy * cr * sp + sy * sr * cp;
	result.z = sy * cr * cp - cy * sr * sp;
	result.w = cy * cr * cp + sy * sr * sp;

	return result;
}

Vector3 CalculateEuler(Vector4 quat)
{
	auto y_p2 = quat.y * quat.y;

	auto x = std::atan2(2.f * (quat.w * quat.z + quat.x * quat.y), (1.f - 2.f * (y_p2 + quat.z * quat.z))) * 57.2957795131f;
	auto y = std::atan2(2.f * (quat.w * quat.x + quat.y * quat.z), (1.f - 2.f * (quat.x * quat.x + y_p2))) * 57.2957795131f;

	return Vector3(x, y, 0.f);
}

Vector3 CalcAngle(Vector3 enemyPos, Vector3 cameraPos)
{
	Vector3 dir = enemyPos - cameraPos;
	float x = asinf(dir.z / dir.Length()) * 57.2957795131f;

	float z = atanf(dir.y / dir.x) * 57.2957795131f;

	if (dir.x >= 0.f) z += 180.f;
	if (x > 179.99f) x -= 360.f;
	else if (x < -179.99f) x += 360.f;

	return Vector3(x, 0.f, z + 90.f);
}

uint64_t GetClosestEntityToCrosshair()
{
	float oDist = FLT_MAX;
	float nDist = 0;
	uint64_t cur = NULL;
	uint64_t target = NULL;
	Vector2 screenPos;
	Vector2 screenCenter = { (float)(GetSystemMetrics(SM_CXSCREEN) / 2), (float)(GetSystemMetrics(SM_CYSCREEN) / 2) };
	float fov = 1000.f; //Here you can change the fov

	for (int i = 0; i < Game.GetEntityCount(); i++)
	{
		cur = Game.GetEntityByID(i);

		if (!Game.IsEnemy(cur)) continue;

		if (Game.WorldToScreen(Game.GetEntityHead(cur), screenPos))
		{
			nDist = DistanceVec2(screenCenter, screenPos);

			if (nDist < fov && nDist < oDist)
			{
				oDist = nDist;
				target = cur;
			}
		}
	}
	return target;
}

uint64_t GetClosestEntityToHead()
{
	float oDist = FLT_MAX;
	float nDist = 0;
	uint64_t cur = NULL;
	uint64_t target = NULL;
	Vector2 screenPos;

	for (int i = 0; i < Game.GetEntityCount(); i++)
	{
		cur = Game.GetEntityByID(i);

		if (!Game.IsEnemy(cur)) continue;

		if (Game.WorldToScreen(Game.GetEntityHead(cur), screenPos))
		{
			nDist = DistanceVec3(Game.GetEntityHead(Game.GetLocalPlayer()), Game.GetEntityHead(cur));

			if (nDist < oDist)
			{
				oDist = nDist;
				target = cur;
			}
		}
	}

	return target;
}

Vector3 calculate_angle(Vector3 translation, Vector3 position)
{
	auto delta = translation - position;

	auto y = -(std::asin(delta.z / fabsf(DistanceVec3(translation, position)))) * 57.2957795131f;
	auto x = std::atan2(delta.y, delta.x) * 57.2957795131f;

	return Vector3(x + 90.f, y, 0.f);
}

Vector3 calculate_euler(Vector4 quaternion)
{
	return Vector3(std::atan2(2.f * (quaternion.w * quaternion.z + quaternion.x * quaternion.y), (1.f - 2.f * (quaternion.y * quaternion.y + quaternion.z * quaternion.z))) * 57.2957795131f,
		std::atan2(2.f * (quaternion.w * quaternion.x + quaternion.y * quaternion.z), (1.f - 2.f * (quaternion.x * quaternion.x + quaternion.y * quaternion.y))) * 57.2957795131f,
		0.f);
}

void SetAngles(bool enable, Vector3 angles, bool silent)
{
	static bool doOnceR = true;
	static bool doOnce = false;

	Vector4 quat = CalculateQuaternion(angles);

	uint64_t holder = Game.DecryptEntityInfo(Game.GetLocalPlayer());
	uint64_t skeleton = Memory.Read<uint64_t>(holder + 0x1270);

	if (enable)
	{
		if (silent)
		{
			/*Memory.Write<uint32_t>(Memory.base + OFFSET_PATCH_PSILENT + 0x3, 0x200); //Old method with patching
			doOnceR = true;
			doOnce = true;
			Memory.Write<Vector4>(skeleton + 0x200, quat);*/
			uint64_t weaponInfo = Game.DecryptWeaponInfo(Game.GetLocalPlayer());
			uint32_t reset = 0x0;
			reset = __ROR4__(reset, 0x1D);
			reset += 0x68;
			Memory.Write<uint32_t>(weaponInfo + 0xC8, reset);
			//printf("Value -> 0x%lX\n", reset); //Should be 0x68 when not wanting reset; 0x862A61CA = 0x50C54BD1, 0x862A61D2 = 0x50C54BD2, 0x862A61C2 = 0x50C54BD0, 0x862A61CA = 0x50C54C2C(Off)
			auto dir = Game.GetEntityHead(GetClosestEntityToCrosshair()) - Game.GetViewTranslation();
			dir.Normalize();
			auto cam_forward = -Game.GetViewForward();
			cam_forward.Normalize();
			Vector3 angles{ atan2(-dir.x, dir.y) - atan2(-cam_forward.x, cam_forward.y), atan2(dir.z, sqrt((dir.x * dir.x) + (dir.y * dir.y))) - atan2(cam_forward.z, sqrt((cam_forward.x * cam_forward.x) + (cam_forward.y * cam_forward.y))), 0.0f };
			Memory.Write<Vector3>(weaponInfo + 0x148, angles);
			doOnce = true;
		}
		else
		{
			/*if (doOnceR)
			{
				Memory.Write<uint32_t>(Memory.base + OFFSET_PATCH_PSILENT + 0x3, 0xC0);
				doOnceR = false;
			}*/
			Memory.Write<Vector4>(skeleton + 0xC0, quat);
		}
	}
	else
	{
		if (doOnce)
		{
			//Memory.Write<uint32_t>(Memory.base + OFFSET_PATCH_PSILENT + 0x3, 0xC0);
			uint64_t weaponInfo = Game.DecryptWeaponInfo(Game.GetLocalPlayer());
			Memory.Write<Vector3>(weaponInfo + 0x148, {0.f, 0.f, 0.f});
			doOnce = false;
		}
	}
}

void Aimbot(bool enable, bool silent)
{
	uint64_t target = 0;
	if (enable)
	{
		//Here you can choose how you want to pick your closest target
		target = GetClosestEntityToCrosshair();

		//target = GetClosestEntityToHead();

		if (!Game.IsEnemy(target)) return;

		Vector3 angles = CalcAngle(Game.GetEntityHead(target), Game.GetViewTranslation());
		angles.Clamp();

		if (GetAsyncKeyState(VK_XBUTTON1) && Game.GetGameState() == 3 && Game.GetEntityHealth(Game.GetLocalPlayer()) >= 20) //You can change the aim key here
		{
			SetAngles(true, angles, silent);
		}
		else
		{
			SetAngles(false, angles, silent);
		}
	}
}