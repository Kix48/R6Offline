#pragma once

#include <Windows.h>
#include <d3dx9.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dwmapi.h>
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dwmapi.lib")
#include "Vector.h"
#include "Game.h"

//variable declaration external to be able to use it in .cpp
//extern IDirect3D9Ex* pObject;
//extern IDirect3DDevice9Ex* pDevice;
//extern D3DPRESENT_PARAMETERS pParam;
//extern ID3DXLine* pLine;
//extern ID3DXFont* pFont;
//extern int Render();
//extern int Width;
//extern int Height;
//extern HWND TargetWnd;
//extern HWND hWnd;
//extern MSG Message;
//extern RECT TargetRect;
//extern const MARGINS Margin;
//
//void OverlayInit(HINSTANCE hInstance);
//
//LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
//
//BOOL RegisterWindowClass(HINSTANCE hInstance);
//
//int DirectXInit(HWND hWnd);
//
//void DirectXShutdown();
//
//void DrawFilledRect(int x, int y, int w, int h, D3DCOLOR color);
//
//void DrawBorderedRect(int x, int y, int w, int h, D3DCOLOR  outercolor, D3DCOLOR  innercolor);
//
//void DrawLine(int x1, int y1, int x2, int y2, int thickness, D3DCOLOR color);
//
//void DrawLine(Vector2 src, Vector2 dst, int thickness, D3DCOLOR color);
//
//void Draw2DBox(Vector2 pos, Vector2 dimensions, int thickness, D3DCOLOR color);
//
//void Draw3DBox(Vector3 top, Vector3 bottom, float angle, int width, int thickness, D3DCOLOR color);
//
//void DrawCircle(int X, int Y, int radius, int numSides, int thickness, D3DCOLOR color);
//
//void DrawString(const char* text, int x, int y, D3DCOLOR color, bool bold);
//
//void DrawStringCentered(const char* text, int x, int y, D3DCOLOR color, bool bold);
//
//int Render();

typedef float D3DVALUE, * LPD3DVALUE;

typedef struct _D3DTLVERTEX
{
    union
    {
        D3DVALUE    sx;             /* Screen coordinates */
        D3DVALUE    dvSX;
    };
    union
    {
        D3DVALUE    sy;
        D3DVALUE    dvSY;
    };
    union
    {
        D3DVALUE    sz;
        D3DVALUE    dvSZ;
    };
    union
    {
        D3DVALUE    rhw;        /* Reciprocal of homogeneous w */
        D3DVALUE    dvRHW;
    };
    union
    {
        D3DCOLOR    color;          /* Vertex color */
        D3DCOLOR    dcColor;
    };
    union
    {
        D3DCOLOR    specular;       /* Specular component of vertex */
        D3DCOLOR    dcSpecular;
    };
    union
    {
        D3DVALUE    tu;             /* Texture coordinates */
        D3DVALUE    dvTU;
    };
    union
    {
        D3DVALUE    tv;
        D3DVALUE    dvTV;
    };
#if(DIRECT3D_VERSION >= 0x0500)
#if (defined __cplusplus) && (defined D3D_OVERLOADS)
    _D3DTLVERTEX() {}
    _D3DTLVERTEX(const D3DVECTOR& v, float _rhw,
        D3DCOLOR _color, D3DCOLOR _specular,
        float _tu, float _tv)
    {
        sx = v.x; sy = v.y; sz = v.z; rhw = _rhw;
        color = _color; specular = _specular;
        tu = _tu; tv = _tv;
    }
#endif
#endif /* DIRECT3D_VERSION >= 0x0500 */
} D3DTLVERTEX, * LPD3DTLVERTEX;

inline HWND hWnd;
inline IDirect3D9* pD3D;
inline IDirect3DDevice9* pDevice;
inline ID3DXFont* pFont1;
inline ID3DXLine* pLine;
const int Width = GetSystemMetrics(SM_CXSCREEN);
const int Height = GetSystemMetrics(SM_CYSCREEN);
static MARGINS margins = { 0, 0, Width, Height };

//Window Stuff
void Render();

WPARAM OverlayThread();

void InitD3D(HWND hwnd);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void SetupWindow();

//Drawing Functions
namespace Colors
{
	extern D3DCOLOR Red;
	extern D3DCOLOR Orange;
	extern D3DCOLOR Yellow;
	extern D3DCOLOR Green;
	extern D3DCOLOR Turquoise;
	extern D3DCOLOR Blue;
	extern D3DCOLOR Purple;
	extern D3DCOLOR Pink;
	extern D3DCOLOR Black;
	extern D3DCOLOR Gray;
	extern D3DCOLOR White;
}

void DrawLine(int x0, int y0, int x1, int y1, D3DCOLOR color);

void DrawRect(int x, int y, int w, int h, D3DCOLOR color);

void DrawFilledRect(int x, int y, int w, int h, D3DCOLOR color);

void DrawHealthbars(int x, int y, int w, int h, int val, int max, D3DCOLOR color);

void DrawOutlinedRect(int x, int y, int w, int h, D3DCOLOR color);

void DrawCircle(int x, int y, int r, int numSides, D3DCOLOR color);

void RotatePoint(Vector2 pointToRotate, Vector2 centerPoint, float angle, Vector2& returnVec, bool angleInRadians);