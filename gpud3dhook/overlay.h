#pragma once
#include <map>

class Overlay
{
public:
	static bool CheckDWM();
	bool CreateClass(const char* classname, WNDPROC wndproc = [](HWND window, UINT msg, WPARAM wp, LPARAM lp) ->LRESULT
	{return DefWindowProc(window, msg, wp, lp); });
	bool CreateOverlay(const char* title = "");
	void MakeTargetWindow(HWND window = nullptr);
	bool InitOverlayDirextX();
	void Render();

	void DrawString(int x, int y, const std::string& text, D3DCOLOR color);
	void DrawRect(int x, int y, int width, int height, D3DCOLOR color);
	void DrawBorderedRect(int x, int y, int width, int height, D3DCOLOR filled, D3DCOLOR color);
	void DrawLine(int x, int y, int x2, int y2, D3DCOLOR color);
	void DrawFilledRect(int x, int y, int width, int height, D3DCOLOR color);
	void DrawCircle(int x, int y, int radius, D3DCOLOR color);

	WNDCLASSEXA overlayClass;
	HWND overlayWindow, targetWindow;
	MSG message;
	MARGINS margin;
	int width, height;
	char className[255];
	HINSTANCE instance;
	
	IDirect3D9Ex* d3d;
	IDirect3DDevice9Ex* device;
	D3DPRESENT_PARAMETERS param;
	ID3DXFont* font;
	ID3DXLine* line;

	std::map<std::string, std::function<bool(IDirect3D9Ex*, IDirect3DDevice9Ex*)>> renderCallbackPre;
	std::map<std::string, std::function<void(IDirect3D9Ex*, IDirect3DDevice9Ex*)>> renderCallbackPost;
	std::map<std::string, std::function<bool(IDirect3D9Ex*, IDirect3DDevice9Ex*)>> swapCallbackPre;
	std::map<std::string, std::function<void(IDirect3D9Ex*, IDirect3DDevice9Ex*)>> swapCallbackPost;
};
