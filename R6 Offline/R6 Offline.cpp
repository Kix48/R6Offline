//Made by Kix

#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <thread>
#include "Overlay.h"
#include "Aimbot.h"
#include "Visuals.h"
#include "Misc.h"

#pragma comment(lib, "Winmm.lib")

namespace Config
{
    inline bool SpoofSpectate = false;
    inline bool RunAndGun = true;
    inline bool Speed = true;
    inline int SpeedVal = 170;
    inline bool FarKnife = true;
    inline bool NoRecoil = true;
    inline bool NoSpread = true;
    inline bool Glow = true;
    inline bool GlowDull = true;
    inline bool CavEsp = true;
    inline bool Aimbot = true;
    inline bool Silent = false;
    inline bool Esp = true;
    inline bool UnlockAll = true;
}

void FeaturesThread()
{
    UnlockAll();

    while (true)
    {
        if (Game.GetGameState() == 2 || Game.GetGameState() == 3)
        {
            SpoofSpectate(Config::SpoofSpectate); //Utd; Set to true if you want
            RunAndGun(Config::RunAndGun); //Utd
            Speed(Config::Speed, Config::SpeedVal); //Utd
            FarKnife(Config::FarKnife); //Utd
            NoRecoil(Config::NoRecoil); //Utd
            NoSpread(Config::NoSpread); //Utd
            Glow(Config::Glow, Config::GlowDull); //Utd
            CaveiraESP(Config::CavEsp); //Utd
        }

        Sleep(1);
    }
}

void AimbotThread()
{
    while (true)
    {
        Aimbot(Config::Aimbot, Config::Silent); //Utd
        Sleep(1);
    }
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    //Console Stuff
    AllocConsole();
    SetConsoleTitleA("R6 Offline | Kix#6496");
    freopen("CONOUT$", "w+", stdout);

    if (Memory.Initialize())
    {
        //ShowWindow(GetConsoleWindow(), SW_HIDE);
        std::thread aimbot(AimbotThread);
        aimbot.detach();
        std::thread features(FeaturesThread);
        features.detach();
        std::thread crosshair(CrosshairCordinates);
        crosshair.detach();
        if(Config::UnlockAll) UnlockAll();
        if (Config::Esp)
        {
            SetupWindow();
            OverlayThread();
        }
        else
        {
            while (!(GetAsyncKeyState(VK_END) & 1))
            {
                Sleep(1);
            }
        }
    }
    else
    {
        printf("Could not initialize memory! Exiting...\n");
        Sleep(3000);
        return 0;
    }

    return 0;
}
