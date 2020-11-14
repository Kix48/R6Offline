#pragma once
#include <thread>
#include <vector>
#include "Game.h"
#include "Overlay.h"


inline float rotation = 0.0f;

inline Vector3 RainbowESP()
{
	static uint32_t cnt = 0;
	float freq = 0.05f;

	if (cnt++ >= (uint32_t)-1)
		cnt = 0;

	Vector3 ret = { std::sin(freq * cnt + 0) * 0.5f + 0.5f, std::sin(freq * cnt + 2) * 0.5f + 0.5f , std::sin(freq * cnt + 4) * 0.5f + 0.5f };
	return ret;
}

inline Vector3 RainbowGlow()
{
	static uint32_t cnt = 0;
	float freq = 0.01f;

	if (cnt++ >= (uint32_t)-1)
		cnt = 0;

	Vector3 ret = { std::sin(freq * cnt + 0) * 0.5f + 0.5f, std::sin(freq * cnt + 2) * 0.5f + 0.5f , std::sin(freq * cnt + 4) * 0.5f + 0.5f };
	return ret;
}

inline void SpottedEsp(bool enable)
{
	Memory.Write<uint8_t>(Memory.base + OFFSET_PATCH_SPOTTED_ESP + 0x6, enable ? 0x7 : 0x0);
}

inline void SpoofSpectate(bool enable)
{
	uint64_t spoof = Memory.Read<uint64_t>(Memory.base + OFFSET_SPOOF_SPECTATE);
	Memory.Write<BYTE>(spoof + 0x5D, enable ? 0x1 : 0x0);
}

inline float BOG_TO_GRD(float BOG)
{
	return (float)(180 / M_PI) * BOG;
}

inline float GRD_TO_BOG(float GRD)
{
	return (float)(M_PI / 180) * GRD;
}

inline void Crosshair(int size, D3DCOLOR color)
{

	POINT Screen;
	Screen.x = GetSystemMetrics(SM_CXSCREEN); Screen.y = GetSystemMetrics(SM_CYSCREEN);
	//Middle POINT
	POINT Middle; Middle.x = (int)(Screen.x / 2); Middle.y = (int)(Screen.y / 2);
	float gamma = atanf(1);

	int rot = (int)rotation;

	int i = 0;
	while (i < 4)
	{
		std::vector<int> p;
		p.push_back((int)(size * sin(GRD_TO_BOG((float)(rot + (i * 90))))));
		p.push_back((int)(size * cos(GRD_TO_BOG((float)(rot + (i * 90))))));

		DrawLine(Middle.x, Middle.y, Middle.x + p[0], Middle.y - p[1], color);

		i++;
	}
}

inline void CrosshairCordinates()
{
	while (true)
	{
		rotation++;
		if ((int)rotation > 89) { rotation = (float)0; }
		rotation++;
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

inline void ESP(bool enable)
{
	const int Width = GetSystemMetrics(SM_CXSCREEN);
	const int Height = GetSystemMetrics(SM_CYSCREEN);

	if (enable && (Game.GetGameState() == 2 || Game.GetGameState() == 3))
	{
		Vector3 rainbowVec = RainbowESP();
		D3DCOLOR rainbowColor = D3DCOLOR_RGBA((int)(rainbowVec.x * 255), (int)(rainbowVec.y * 255), (int)(rainbowVec.z * 255), 255);

		//Crosshair
		Crosshair(20, rainbowColor);

		/*int crosshairSize = 20;
		DrawLine((int)(Width / 2) - crosshairSize, (int)(Height / 2), (int)(Width / 2) + crosshairSize, (int)(Height / 2), rainbowColor);
		DrawLine((int)(Width / 2), (int)(Height / 2) - crosshairSize, (int)(Width / 2), (int)(Height / 2) + crosshairSize, rainbowColor);*/

		for (int i = 0; i < Game.GetEntityCount(); i++)
		{
			uint64_t cur = Game.GetEntityByID(i);
			if (!Game.IsEnemy(cur)) continue;

			//Snaplines
			Vector3 feetPos3D = Game.GetEntityPos(cur);

			Vector2 feetPos2D;
			if (Game.WorldToScreen(feetPos3D, feetPos2D))
			{
				DrawLine(Width / 2, Height, (int)feetPos2D.x, (int)feetPos2D.y, rainbowColor /*Colors::Red*/);
			}

			//Head Circle
			Vector3 headPos3D = Game.GetEntityHead(cur);

			Vector2 headPos2D;
			if (Game.WorldToScreen(headPos3D, headPos2D))
			{
				DrawCircle((int)headPos2D.x, (int)headPos2D.y, 3, 10, rainbowColor);
			}

			//Box Esp
			headPos3D.z += 0.2f;
			Vector2 boxTop2D;
			if (Game.WorldToScreen(headPos3D, boxTop2D))
			{
				float boxHeight = feetPos2D.y - boxTop2D.y;
				float boxWidth = boxHeight / 2.4f;

				DrawOutlinedRect((int)(boxTop2D.x - (boxWidth / 2.f)), (int)boxTop2D.y, (int)boxWidth, (int)boxHeight, rainbowColor /*Colors::Red*/);

				//Healthbar
				float dX = feetPos2D.x - headPos2D.x;

				float healthPercentage = (float)(Game.GetEntityHealth(cur)) / 100.f;

				Vector2 topHealth, bottomHealth;
				float healthHeight = boxHeight * healthPercentage;

				bottomHealth.x = boxTop2D.x + (boxWidth / 2) + 6;
				bottomHealth.y = boxTop2D.y + boxHeight;

				topHealth.x = boxTop2D.x + (boxWidth / 2) + 6;
				topHealth.y = boxTop2D.y + boxHeight - healthHeight;

				DrawLine((int)bottomHealth.x, (int)bottomHealth.y, (int)topHealth.x, (int)topHealth.y, Colors::Green);

				/*DrawHealthbars(boxTop2D.x + (boxWidth / 2.f) + 6, boxTop2D.y, 2, boxHeight, Game.GetEntityHealth(cur), 100, Colors::Green);
				printf("Health %d: %d\n", i, Game.GetEntityHealth(cur));*/
				//draw_outlined_rect(box_top.x - box_width / 2, box_top.y, box_width, box_height, epic_blue);
				//draw_healthbars(box_top.x - box_width / 2 - 6, box_top.y, 2, box_height, ent.m_health, 100, epic_blue);
			}
		}
	}
}

inline void Glow(bool enable, bool dull)
{
	float d = 100.f;
	float s = 2.f;
	float a = 4.0f; //Doesnt do much
	float o;
	if (dull)
	{
		o = 0.001f; //Dull
	}
	else
	{
		o = 5.f; //Bright
	}

	if (enable)
	{
		Vector3 rainbowVec = RainbowGlow();;
		if (dull)
		{
			rainbowVec.x *= 2.55f; //Dull
			rainbowVec.y *= 2.55f;
			rainbowVec.z *= 2.55f;
		}
		else
		{
			rainbowVec.x *= 255.f; //Bright
			rainbowVec.y *= 255.f;
			rainbowVec.z *= 255.f;
		}

		uint64_t glow = Memory.Read<uint64_t>(Memory.base + OFFSET_GLOW_MANAGER);
		glow = Memory.Read<uint64_t>(glow + 0xB8);

		//Color
		Memory.Write<Vector3>(glow + 0xD0, rainbowVec);

		//Distance
		uint32_t distance = FloatToHex(d);
		distance += 0x35;
		Memory.Write<uint32_t>(glow + 0x110, distance);

		//Start
		uint32_t start = FloatToHex(s);
		start = ((start - 0x30) ^ 0x2E85EB10) - 0x69C11B36;
		Memory.Write<uint32_t>(glow + 0x114, start);

		//Alpha
		uint32_t alpha = FloatToHex(a);
		alpha = ((alpha - 0x8C6CCD9) ^ 0x4E) + 0x1B;
		Memory.Write<uint32_t>(glow + 0x118, alpha);

		//Opacity
		uint32_t opacity = FloatToHex(o);
		opacity = __ROR4__((__ROR4__(opacity, 0x5) ^ 0xE0FB87A), 0x19);
		Memory.Write<uint32_t>(glow + 0x11C, opacity);
	}
}

inline void CaveiraESP(bool enable)
{
	static bool doOnce = false;
	if (enable)
	{
		for (int i = 0; i < Game.GetEntityCount(); i++)
		{
			auto entityObject = Game.GetEntityByID(i);
			entityObject = Game.DecryptEntityInfo(entityObject);

			auto entityInfo = Memory.Read<uint64_t>(entityObject + 0x18);
			entityInfo = Memory.Read<uint64_t>(entityInfo + 0xD8);

			for (uint32_t curCo = 0x80; curCo < 0xF0; curCo += 4)
			{
				auto marker = Memory.Read<uint64_t>(entityInfo + curCo);
				if (marker == 0)
					continue;

				auto markerCheck = Memory.Read<uint64_t>(marker);
				if (markerCheck != (Memory.base + 0x52C71F0))
					continue;


				Memory.Write<uint8_t>(marker + 0x220, 0x85);
			}
		}
		doOnce = true;
	}
	else
	{
		if (doOnce)
		{
			for (int i = 0; i < Game.GetEntityCount(); i++)
			{
				auto entityObject = Game.GetEntityByID(i);
				entityObject = Game.DecryptEntityInfo(entityObject);

				auto entityInfo = Memory.Read<uint64_t>(entityObject + 0x18);
				entityInfo = Memory.Read<uint64_t>(entityInfo + 0xD8);

				for (uint32_t curCo = 0x80; curCo < 0xF0; curCo += 4)
				{
					auto marker = Memory.Read<uint64_t>(entityInfo + curCo);
					if (marker == 0)
						continue;

					auto markerCheck = Memory.Read<uint64_t>(marker);
					if (markerCheck != (Memory.base + 0x52C71F0))
						continue;


					Memory.Write<uint8_t>(marker + 0x220, 0x84);
				}
			}
			doOnce = false;
		}
	}
}