#include "Overlay.h"
#include "Visuals.h"

WPARAM OverlayThread()
{
	static bool pauseMusic = false;
	static bool pauseOnce = true;
	static bool resumeOnce = false;
	MSG message = { 0 };
	RECT rect;
	ZeroMemory(&rect, sizeof(RECT));
	GetWindowRect(FindWindowA(NULL, "Rainbow Six"), &rect);

	while (!(GetAsyncKeyState(VK_END) & 1))
	{

		ZeroMemory(&message, sizeof(MSG));

		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		if (message.message == WM_QUIT) { exit(0); }

		if (!FindWindowA(NULL, "Rainbow Six")) break;

		MoveWindow(hWnd, rect.left, rect.top, Width, Height, true);

		HWND hWindow2 = GetForegroundWindow();
		HWND hWindow3 = GetWindow(hWindow2, GW_HWNDPREV);

		SetWindowPos(hWnd, hWindow3, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		UpdateWindow(hWnd);

		Render();

		Sleep(1);
	}

	return message.wParam;
}

void InitD3D(HWND hwnd)
{
	pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	D3DPRESENT_PARAMETERS direct3DPresetParams;
	ZeroMemory(&direct3DPresetParams, sizeof(direct3DPresetParams));

	direct3DPresetParams.Windowed = true;
	direct3DPresetParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	direct3DPresetParams.hDeviceWindow = hwnd;
	direct3DPresetParams.BackBufferFormat = D3DFMT_A8R8G8B8;
	direct3DPresetParams.BackBufferWidth = Width;
	direct3DPresetParams.BackBufferHeight = Height;
	direct3DPresetParams.EnableAutoDepthStencil = TRUE;
	direct3DPresetParams.AutoDepthStencilFormat = D3DFMT_D16;

	pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &direct3DPresetParams, &pDevice);
	D3DXCreateFontA(pDevice, 12, NULL, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Tahoma", &pFont1);
	D3DXCreateLine(pDevice, &pLine);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		DwmExtendFrameIntoClientArea(hWnd, &margins);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProcA(hWnd, message, wParam, lParam);
}

void SetupWindow()
{
	RECT rect;
	GetWindowRect(FindWindowA(NULL, "Rainbow Six"), &rect);

	WNDCLASSEX windowClass;
	ZeroMemory(&windowClass, sizeof(WNDCLASSEX));

	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WndProc;
	windowClass.hInstance = GetModuleHandleA(0);
	windowClass.hCursor = LoadCursorA(NULL, IDC_ARROW);
	windowClass.hbrBackground = (HBRUSH)RGB(0, 0, 0);
	windowClass.lpszClassName = "R6 Offline";
	RegisterClassExA(&windowClass);

	hWnd = CreateWindowExA(WS_EX_LAYERED | WS_EX_TRANSPARENT, windowClass.lpszClassName, "R6 Offline", WS_POPUP, rect.left, rect.top, Width, Height, NULL, NULL, windowClass.hInstance, NULL);

	SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), 0, ULW_COLORKEY);
	SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);

	ShowWindow(hWnd, SW_SHOW);
	InitD3D(hWnd);
}

D3DCOLOR Colors::Red = D3DCOLOR_RGBA(255, 0, 0, 255);
D3DCOLOR Colors::Orange = D3DCOLOR_RGBA(255, 100, 0, 255);
D3DCOLOR Colors::Yellow = D3DCOLOR_RGBA(255, 255, 0, 255);
D3DCOLOR Colors::Green = D3DCOLOR_RGBA(0, 255, 0, 255);
D3DCOLOR Colors::Turquoise = D3DCOLOR_RGBA(0, 255, 255, 255);
D3DCOLOR Colors::Blue = D3DCOLOR_RGBA(0, 0, 255, 255);
D3DCOLOR Colors::Purple = D3DCOLOR_RGBA(130, 0, 255, 255);
D3DCOLOR Colors::Pink = D3DCOLOR_RGBA(255, 0, 255, 255);
D3DCOLOR Colors::Black = D3DCOLOR_RGBA(0, 0, 0, 255);
D3DCOLOR Colors::Gray = D3DCOLOR_RGBA(69, 69, 69, 255);
D3DCOLOR Colors::White = D3DCOLOR_RGBA(255, 255, 255, 255);

void DrawLine(int x0, int y0, int x1, int y1, D3DCOLOR color)
{
	D3DXVECTOR2 lines[2] = {
		D3DXVECTOR2((FLOAT)x0, (FLOAT)y0),
		D3DXVECTOR2((FLOAT)x1, (FLOAT)y1)
	};
	pLine->Begin();
	pLine->Draw(lines, 2, color);
	pLine->End();
}

void DrawRect(int x, int y, int w, int h, D3DCOLOR color)
{
	DrawLine(x, y, x + w, y, color);
	DrawLine(x, y, x, y + h, color);
	DrawLine(x + w, y, x + w, y + h, color);
	DrawLine(x, y + h, x + w + 1, y + h, color);
}

void DrawFilledRect(int x, int y, int w, int h, D3DCOLOR color)
{
	D3DTLVERTEX qV[4] = {
		{ (float)x, (float)y + (float)h, 0.f, 1.f, color },
		{ (float)x, (float)y, 0.f, 1.f, color },
		{ (float)x + (float)w, (float)y + (float)h, 0.f, 1.f, color },
		{ (float)x + (float)w, (float)y , 0.f, 1.f, color }
	};
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	pDevice->SetTexture(0, nullptr);
	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, qV, sizeof(D3DTLVERTEX));
}

void DrawOutlinedRect(int x, int y, int w, int h, D3DCOLOR color)
{
	DrawRect(x - 1, y - 1, w + 2, h + 2, Colors::Black);
	DrawRect(x + 1, y + 1, w - 2, h - 2, Colors::Black);
	DrawRect(x, y, w, h, color);
}

void DrawHealthbars(int x, int y, int w, int h, int val, int max, D3DCOLOR color)
{
	DrawRect(x, y - 1, w + 1, h + 2, Colors::Black);
	DrawFilledRect(x, y, w, val / max * h, color);
}

void DrawCircle(int x, int y, int r, int numSides, D3DCOLOR color)
{
	D3DXVECTOR2 Line[128];
	double Step = M_PI * 2.0 / numSides;
	int Count = 0;
	for (double a = 0; a < M_PI * 2.0; a += Step)
	{
		float X1 = r * (float)cos(a) + (float)x;
		float Y1 = r * (float)sin(a) + (float)y;
		float X2 = r * (float)cos(a + Step) + (float)x;
		float Y2 = r * (float)sin(a + Step) + (float)y;
		Line[Count].x = X1;
		Line[Count].y = Y1;
		Line[Count + 1].x = X2;
		Line[Count + 1].y = Y2;
		Count += 2;
	}
	pLine->Draw(Line, Count, color);
}

void RotatePoint(Vector2 pointToRotate, Vector2 centerPoint, float angle, Vector2& returnVec, bool angleInRadians)
{
	if (!angleInRadians)
		angle = (float)(angle * (M_PI / 180));
	float cosTheta = (float)cosf(angle);
	float sinTheta = (float)sinf(angle);

	returnVec = {
		cosTheta * (pointToRotate.x - centerPoint.x) - sinTheta * (pointToRotate.y - centerPoint.y),
		sinTheta * (pointToRotate.x - centerPoint.x) + cosTheta * (pointToRotate.y - centerPoint.y) };

	returnVec += centerPoint;
}

void Render()
{
	pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	pDevice->BeginScene();

	//Draw
	ESP(true);

	pDevice->EndScene();
	pDevice->Present(NULL, NULL, NULL, NULL);
}