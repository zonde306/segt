#pragma once

#include <memory>
#include <string>
#include <vector>
#include <d3d9.h>
#include <d3dx9.h>

struct IDirect3DDevice9;
struct IDirect3DStateBlock9;
struct ID3DXFont;

#undef CreateFont

class DrawManager
{
public:
     DrawManager(IDirect3DDevice9* pDevice);
     ~DrawManager();

     void OnLostDevice();
     void OnResetDevice();

     void BeginRendering();
     void EndRendering();

     void RenderLine(D3DCOLOR color, int x1, int y1, int x2, int y2);
     void RenderRect(D3DCOLOR color, int x, int y, int w, int h);
     void RenderCircle(D3DCOLOR color, int x, int y, int r, int resolution = 64);
     void RenderText(D3DCOLOR color, int x, int y, bool centered, const char* fmt, ...);
     void FillRect(D3DCOLOR color, int x, int y, int w, int h);

	 IDirect3DTexture9* GenerateTexture(D3DCOLOR color, UINT width = 8, UINT height = 8);

	 void DrawString(int x, int y, const std::string& text, D3DCOLOR color);
	 void DrawString(int x, int y, D3DCOLOR color, const char* text, ...);
	 void DrawRect(int x, int y, int width, int height, D3DCOLOR color);
	 void DrawBorderedRect(int x, int y, int width, int height, D3DCOLOR filled, D3DCOLOR color);
	 void DrawLine(int x, int y, int x2, int y2, D3DCOLOR color);
	 void DrawFilledRect(int x, int y, int width, int height, D3DCOLOR color);
	 void DrawCircle(int x, int y, int radius, D3DCOLOR color);

     /*
     * Below are some functions that you can implement yourself as an exercise
     *
     * void FillCircle(DWORD color, int x, int y, int r, int resolution = 64);
     * void RenderPolygon(DWORD color, int* x, int* y, int npoints);
     * void FillPolygon(DWORD color, int* x, int* y, int npoints);
     * void FillGradient(DWORD from, Color to, bool isVertical, int x, int y, int w, int h);
     */

private:
     void ReleaseObjects();
     void CreateObjects();

private:
     IDirect3DDevice9*		m_pDevice;
     IDirect3DStateBlock9*	m_pStateBlock;
     ID3DXFont*				m_pDefaultFont;
	 ID3DXLine*				m_pLine;
};