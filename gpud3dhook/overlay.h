#pragma once

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
};
