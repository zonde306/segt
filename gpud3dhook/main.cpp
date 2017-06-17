#include "main.h"


//---------------
//----PLAYERS----
//---------------

#define l4d2_ellisbody(_s,_n,_p)	(_s == 32 && _n == 4822 && _p == 8088) 
#define l4d2_ellishead(_s,_n,_p)	(_s == 32 && ((_n == 2037 && _p == 3832) || (_n == 2661 && _p == 4872))) 
#define l4d2_ellis(_s,_n,_p)		(l4d2_ellisbody || l4d2_ellishead)

#define l4d2_coatchbody(_s,_n,_p)	(_s == 32 && _n == 5710 && _p == 9262) 
#define l4d2_coatchead(_s,_n,_p)	(_s == 32 && ( (_n == 1819 && _p == 3375) || (_n == 2387 && _p == 4527) ))
#define l4d2_coatch(_s,_n,_p)		(l4d2_coatchbody || l4d2_coatchead )

#define l4d2_rachellebody(_s,_n,_p)	(_s == 32 && _n == 6443 && _p == 10138) 
#define l4d2_rachellehead(_s,_n,_p)	(_s == 32 && ( (_n == 1647 && _p == 3069) || (_n == 3947 && _p == 6910) ))
#define l4d2_rachelle(_s,_n,_p)		(l4d2_rachellebody || l4d2_rachellehead )

#define l4d2_nickbody(_s,_n,_p)		(_s == 32 && ( (_n == 7957 && _p == 11314) || (_n == 7913 && _p == 12247) )) 
#define l4d2_nickhead(_s,_n,_p)		(_s == 32 && ( (_n == 1680 && _p == 3133) || (_n == 2384 && _p == 4540) )) 
#define l4d2_nick(_s,_n,_p)			(l4d2_nickbody || l4d2_nickhead)

#define l4d2_survivor(_s,_n,_p)		(l4d2_ellis(_s,_n,_p) || l4d2_coatch(_s,_n,_p) || l4d2_rachelle(_s,_n,_p) || l4d2_nick(_s,_n,_p))

#define l4d2_pistolhands (_s == 32 && _n == 10318 && _p == 9340) 
#define l4d2_hands (_s == 32 && _n == 4262 && _p == 6468) 


//---------------
//----WEAPONS---
//--------------


//GUNS
#define l4d2_p220(_s,_n,_p)			(_s == 32 && _n == 3406 && _p == 2883)
#define l4d2_glock(_s,_n,_p)		(_s == 32 && _n == 961 && _p == 835) 
#define l4d2_deagle(_s,_n,_p)		(_s == 32 && _n == 2530 && _p == 1962) 

#define l4d2_guns(_s,_n,_p)			(l4d2_p220(_s,_n,_p) || l4d2_glock(_s,_n,_p) || l4d2_deagle(_s,_n,_p))

//SHOTGUNS
#define l4d2_spas12(_s,_n,_p)		(_s == 32 && _n == 5320 && _p == 5277) 
#define l4d2_chrome(_s,_n,_p)		(_s == 32 && _n == 3525 && _p == 3418) 
#define l4d2_autoshotgun(_s,_n,_p)	(_s == 32 && _n == 2942 && _p == 2224)  
#define l4d2_pumpshotgun(_s,_n,_p)	(_s == 32 && _n == 2507 && _p == 2362) 

#define l4d2_shotguns(_s,_n,_p)		(l4d2_spas12(_s,_n,_p) || l4d2_chrome(_s,_n,_p) || l4d2_autoshotgun(_s,_n,_p) || l4d2_pumpshotgun(_s,_n,_p))

//SMG
#define l4d2_uzi(_s,_n,_p)			(_s == 32 && _n == 2318 && _p == 1875)
#define l4d2_uzisilent(_s,_n,_p)	(_s == 32 && _n == 5283 && _p == 5221)
#define l4d2_mp5(_s,_n,_p)			(_s == 32 && _n == 502 && _p == 410) 

#define l4d2_smg(_s,_n,_p)			(l4d2_uzi(_s,_n,_p) || l4d2_uzisilent(_s,_n,_p) || l4d2_mp5(_s,_n,_p))

//MELEE
#define l4d2_axe(_s,_n,_p)			(_s == 32 && _n == 1594 && _p == 2638) //hache
#define l4d2_crowbar(_s,_n,_p)		(_s == 32 && _n == 1061 && _p == 1510) //barredefer
#define l4d2_baseballbat(_s,_n,_p)	(_s == 32 && _n == 641 && _p == 1132)
#define l4d2_chainsaw(_s,_n,_p)		(_s == 32 && _n == 4880 && _p == 4287) //tronconneuse
#define l4d2_katana(_s,_n,_p)		(_s == 32 && _n == 950 && _p == 1179) 
#define l4d2_cricketbat(_s,_n,_p)	(_s == 32 && _n == 1354 && _p == 2103) 

#define l4d2_melee(_s,_n,_p)		(l4d2_axe(_s,_n,_p) || l4d2_crowbar(_s,_n,_p) || l4d2_baseballbat(_s,_n,_p) || l4d2_chainsaw(_s,_n,_p) || l4d2_katana(_s,_n,_p) || l4d2_cricketbat(_s,_n,_p))

//RIFLES
#define l4d2_m4(_s,_n,_p)			(_s == 32 && _n == 2562 && _p == 2018) 
#define l4d2_scar(_s,_n,_p)			(_s == 32 && _n == 4878 && _p == 3765) 
#define l4d2_ak47(_s,_n,_p)			(_s == 32 && _n == 3574 && _p == 2817)
#define l4d2_m60(_s,_n,_p)			(_s == 32 && _n == 6366 && _p == 4785) 
#define l4d2_sig552(_s,_n,_p)		(_s == 32 && _n == 573 && _p == 417)

#define l4d2_rifles(_s,_n,_p) (l4d2_m4(_s,_n,_p) || l4d2_scar(_s,_n,_p) || l4d2_ak47(_s,_n,_p) || l4d2_m60(_s,_n,_p) || l4d2_sig552(_s,_n,_p))

//SNIPERS
#define l4d2_awp(_s,_n,_p)			(_s == 32 && _n == 681 && _p == 523) 
#define l4d2_scout(_s,_n,_p)		(_s == 32 && _n == 606 && _p == 477) 
#define l4d2_military(_s,_n,_p)		(_s == 32 && _n == 4278 && _p == 4059)
#define l4d2_huntingrifle(_s,_n,_p)	(_s == 32 && _n == 2514 && _p == 2069) 

#define l4d2_snipers(_s,_n,_p)		(l4d2_awp(_s,_n,_p) || l4d2_scout(_s,_n,_p) || l4d2_militarysniper(_s,_n,_p) || l4d2_huntingrifle(_s,_n,_p))

//GRENADELAUNCHER
#define l4d2_grenade(_s,_n,_p)		(_n == 2575 && _p == 2265) 

#define l4d2_weapons(_s,_n,_p)		(l4d2_guns(_s,_n,_p) || l4d2_shotguns(_s,_n,_p) || l4d2_smg(_s,_n,_p) || l4d2_melee(_s,_n,_p) || l4d2_rifles(_s,_n,_p) || l4d2_snipers(_s,_n,_p) || l4d2_grenade(_s,_n,_p))

//---------------
//----STUFF------
//---------------

#define l4d2_medickit(_s,_n,_p)		(_s == 32 && _n == 883 && _p == 902) 
#define l4d2_adrenaline(_s,_n,_p)	(_s == 32 && _n == 2035 && _p == 1842) 
#define l4d2_defib(_s,_n,_p)		(_s == 32 && _n == 4901 && _p == 5885) 
#define l4d2_painpills(_s,_n,_p)	(_s == 32 && _n == 240 && _p == 282) //(cachet)
#define l4d2_gascan(_s,_n,_p)		(_s == 32 && _n == 855 && _p == 952) //gericane
#define l4d2_propanetank(_s,_n,_p)	(_s == 32 && _n == 1105 && _p == 1092) //bombone de prop
#define l4d2_oxygentank(_s,_n,_p)	(_s == 32 && _n == 1779 && _p == 1742) //bombone d'oxy
#define l4d2_fireworkbox(_s,_n,_p)	(_s == 32 && _n == 222 && _p == 334)
#define l4d2_pipebomb(_s,_n,_p)		(_s == 32 && _n == 2375 && _p == 1956) 
#define l4d2_bilebomb(_s,_n,_p)		(_s == 32 && ( (_n == 98 && _p == 192) || (_n == 1526 && _p == 1491) )) 
#define l4d2_molotov(_s,_n,_p)		(_s == 32 && ( (_n == 442 && _p == 576) || (_n == 200 && _p == 296) )) 
#define l4d2_cola(_s,_n,_p)			(_s == 32 && ( (_n == 1353 && _p == 1337) || (_n == 930 && _p == 1446) ))
#define l4d2_gnome(_s,_n,_p)		(_s == 32 && _n == 1648 && _p == 2088)
#define l4d2_explosif(_s,_n,_p)		(_s == 32 && _n == 815 && _p == 736) //explosif bullet box
#define l4d2_incendiary(_s,_n,_p)	(_s == 32 && _n == 821 && _p == 736) //incendiary bullet box

#define l4d2_stuff(_s,_n,_p)		(l4d2_medickit(_s,_n,_p) || l4d2_adrenaline(_s,_n,_p) || l4d2_defib(_s,_n,_p) || l4d2_painpills(_s,_n,_p) || l4d2_gascan(_s,_n,_p) || l4d2_propanetank(_s,_n,_p) || l4d2_oxygentank(_s,_n,_p) || l4d2_fireworkbox(_s,_n,_p) || l4d2_pipebomb(_s,_n,_p) || l4d2_bilebomb(_s,_n,_p) || l4d2_molotov(_s,_n,_p) || l4d2_cola(_s,_n,_p) || l4d2_gnome(_s,_n,_p) || l4d2_explosif(_s,_n,_p) || l4d2_incendiary(_s,_n,_p))


//---------------
//----ZOMBIE-----
//---------------

//INFECTED
#define l4d2_zombie1body(_s,_n,_p)	(_s == 32 && _n == 2035 && _p == 3038)
#define l4d2_zombie1head(_s,_n,_p)	(_s == 32 && _n == 834 && _p == 1289) 
#define l4d2_zombie2body(_s,_n,_p)	(_s == 32 && _n == 1673 && _p == 2608) 
#define l4d2_zombie2head(_s,_n,_p)	(_s == 32 && _n == 854 && _p == 1389) 
#define l4d2_zombie3body(_s,_n,_p)	(_s == 32 && _n == 1732 && _p == 2581)

//(NOT WORKING) too many zombies models, too lazy to log all of them :p

#define l4d2_common(_s,_n,_p)		(l4d2_zombie1body(_s,_n,_p) || l4d2_zombie1head(_s,_n,_p) || l4d2_zombie2body(_s,_n,_p) || l4d2_zombie2head(_s,_n,_p) || l4d2_zombie3body(_s,_n,_p))

//SPECIAL ZOMBIE
#define l4d2_tank(_s,_n,_p)			(_s == 32 && _n == 3600 && _p == 5812) 
#define l4d2_smoker(_s,_n,_p)		(_s == 32 && _n == 6571 && _p == 11361) 
#define l4d2_hunter(_s,_n,_p)		(_s == 32 && _n == 6896 && _p == 11701) 
#define l4d2_boomer(_s,_n,_p)		(_s == 32 && _n == 3248 && _p == 5257) 
#define l4d2_charger(_s,_n,_p)		(_s == 32 && _n == 3522 && _p == 6071) 
#define l4d2_jockey(_s,_n,_p)		(_s == 32 && ( (_n == 6340 && _p == 10382) || (_n == 6340 && _p == 504) ))
#define l4d2_witch(_s,_n,_p)		(_s == 32 && ( (_n == 3362 && _p == 5099) || (_n == 488 && _p == 566) ))
#define l4d2_spitter(_s,_n,_p)		(_s == 32 && ( (_n == 4811 && _p == 5621) || (_n == 345 && _p == 534) || (_n == 4811 && _p == 2381) ))

#define l4d2_special(_s,_n,_p)		(l4d2_tank(_s,_n,_p) || l4d2_smoker(_s,_n,_p) || l4d2_hunter(_s,_n,_p) || l4d2_boomer(_s,_n,_p) || l4d2_charger(_s,_n,_p) || l4d2_jockey(_s,_n,_p) || l4d2_witch(_s,_n,_p) || l4d2_spitter(_s,_n,_p))

#define l4d2_zombies(_s,_n,_p)		(l4d2_common(_s,_n,_p) || l4d2_special(_s,_n,_p))

NAMESPACE_START(dh)

static HWND gFakeWindow = nullptr;
static bool gIsInitialization = false;
D3DPFNHooker gDetourFunc, gDetourFunc1;
IDirect3D9* gD3D9Internal = nullptr;
IDirect3DDevice9* gDeviceInternal = nullptr;
FnOpenAdapter* oOpenAdapter = nullptr;
PFND3DDDI_CREATEDEVICE oCreateDevice = nullptr;
PFND3DDDI_DRAWINDEXEDPRIMITIVE oDrawIndexedPrimitive = nullptr;
PFND3DDDI_SETRENDERSTATE oSetRenderState = nullptr;
PFND3DDDI_CREATEQUERY oCreateQuery = nullptr;
PFND3DDDI_SETSTREAMSOURCE oSetStreamSource = nullptr;
PFND3DDDI_PRESENT oPresent;
PFND3DDDI_LOCK oLock;
PFND3DDDI_CREATERESOURCE oCreateResource;
PFND3DDDI_UNLOCK oUnlock;
PFND3DDDI_BLT oBlt;

static HRESULT APIENTRY Hooked_DrawIndexedPrimitive(HANDLE, D3DDDIARG_DRAWINDEXEDPRIMITIVE*);
static HRESULT APIENTRY Hooked_SetStreamSource(HANDLE, D3DDDIARG_SETSTREAMSOURCE*);
static HRESULT APIENTRY Hooked_CreateQuery(HANDLE, D3DDDIARG_CREATEQUERY*);
static HRESULT APIENTRY Hooked_Present(HANDLE, D3DDDIARG_PRESENT*);
static HRESULT APIENTRY Hooked1_DrawIndexedPrimitive(HANDLE, D3DDDIARG_DRAWINDEXEDPRIMITIVE*);
static HRESULT APIENTRY Hooked1_SetStreamSource(HANDLE, D3DDDIARG_SETSTREAMSOURCE*);
static HRESULT APIENTRY Hooked1_CreateQuery(HANDLE, D3DDDIARG_CREATEQUERY*);
static HRESULT APIENTRY Hooked1_Present(HANDLE, D3DDDIARG_PRESENT*);

static void StartStrideChange()
{
	for (;;)
	{
		if (GetAsyncKeyState(VK_PRIOR) & 0x1)
		{
			
		}
		if (GetAsyncKeyState(VK_NEXT) & 0x1)
		{
			
		}
		if (GetAsyncKeyState(VK_PAUSE) & 0x1)
		{
			
		}
	}

}

static HRESULT APIENTRY Hooked_CreateDevice(HANDLE hAdapter, D3DDDIARG_CREATEDEVICE *pDeviceData)
{
	HRESULT result = oCreateDevice(hAdapter, pDeviceData);

	if (pDeviceData->pDeviceFuncs->pfnDrawIndexedPrimitive != nullptr)
	{
		// 开始 Hook 各类有用的函数
		if (oDrawIndexedPrimitive == nullptr)
		{
			oDrawIndexedPrimitive = pDeviceData->pDeviceFuncs->pfnDrawIndexedPrimitive;
			oSetRenderState = pDeviceData->pDeviceFuncs->pfnSetRenderState;
			oSetStreamSource = pDeviceData->pDeviceFuncs->pfnSetStreamSource;
			oCreateQuery = pDeviceData->pDeviceFuncs->pfnCreateQuery;
			oPresent = pDeviceData->pDeviceFuncs->pfnPresent;
			oLock = pDeviceData->pDeviceFuncs->pfnLock;
			oCreateResource = pDeviceData->pDeviceFuncs->pfnCreateResource;
			oUnlock = pDeviceData->pDeviceFuncs->pfnUnlock;
			oBlt = pDeviceData->pDeviceFuncs->pfnBlt;

			printf_s("oDrawIndexedPrimitive = 0x%X\n", (DWORD)oDrawIndexedPrimitive);
			printf_s("oSetRenderState = 0x%X\n", (DWORD)oSetRenderState);
			printf_s("oSetStreamSource = 0x%X\n", (DWORD)oSetStreamSource);
			printf_s("oCreateQuery = 0x%X\n", (DWORD)oCreateQuery);
			printf_s("oPresent = 0x%X\n", (DWORD)oPresent);
			printf_s("oLock = 0x%X\n", (DWORD)oLock);
			printf_s("oCreateResource = 0x%X\n", (DWORD)oCreateResource);
			printf_s("oUnlock = 0x%X\n", (DWORD)oUnlock);
			printf_s("oBlt = 0x%X\n", (DWORD)oBlt);
		}
		
		if (gDetourFunc.oDrawIndexedPrimitive == nullptr && gDetourFunc1.oSetRenderState != pDeviceData->pDeviceFuncs->pfnSetRenderState)
		{
			gDetourFunc.oDrawIndexedPrimitive = pDeviceData->pDeviceFuncs->pfnDrawIndexedPrimitive;
			gDetourFunc.oSetRenderState = pDeviceData->pDeviceFuncs->pfnSetRenderState;
			gDetourFunc.oSetStreamSource = pDeviceData->pDeviceFuncs->pfnSetStreamSource;
			gDetourFunc.oCreateQuery = pDeviceData->pDeviceFuncs->pfnCreateQuery;
			gDetourFunc.oPresent = pDeviceData->pDeviceFuncs->pfnPresent;
			gDetourFunc.oLock = pDeviceData->pDeviceFuncs->pfnLock;
			gDetourFunc.oCreateResource = pDeviceData->pDeviceFuncs->pfnCreateResource;
			gDetourFunc.oUnlock = pDeviceData->pDeviceFuncs->pfnUnlock;
			gDetourFunc.oBlt = pDeviceData->pDeviceFuncs->pfnBlt;

			pDeviceData->pDeviceFuncs->pfnDrawIndexedPrimitive = (PFND3DDDI_DRAWINDEXEDPRIMITIVE)Hooked_DrawIndexedPrimitive;
			pDeviceData->pDeviceFuncs->pfnSetStreamSource = (PFND3DDDI_SETSTREAMSOURCE)Hooked_SetStreamSource;
			pDeviceData->pDeviceFuncs->pfnCreateQuery = (PFND3DDDI_CREATEQUERY)Hooked_CreateQuery;
			pDeviceData->pDeviceFuncs->pfnPresent = (PFND3DDDI_PRESENT)Hooked_Present;
		}

		if (gDetourFunc1.oDrawIndexedPrimitive == nullptr && gDetourFunc.oSetRenderState != pDeviceData->pDeviceFuncs->pfnSetRenderState)
		{
			gDetourFunc1.oDrawIndexedPrimitive = pDeviceData->pDeviceFuncs->pfnDrawIndexedPrimitive;
			gDetourFunc1.oSetRenderState = pDeviceData->pDeviceFuncs->pfnSetRenderState;
			gDetourFunc1.oSetStreamSource = pDeviceData->pDeviceFuncs->pfnSetStreamSource;
			gDetourFunc1.oCreateQuery = pDeviceData->pDeviceFuncs->pfnCreateQuery;
			gDetourFunc1.oPresent = pDeviceData->pDeviceFuncs->pfnPresent;
			gDetourFunc1.oLock = pDeviceData->pDeviceFuncs->pfnLock;
			gDetourFunc1.oCreateResource = pDeviceData->pDeviceFuncs->pfnCreateResource;
			gDetourFunc1.oUnlock = pDeviceData->pDeviceFuncs->pfnUnlock;
			gDetourFunc1.oBlt = pDeviceData->pDeviceFuncs->pfnBlt;

			pDeviceData->pDeviceFuncs->pfnDrawIndexedPrimitive = (PFND3DDDI_DRAWINDEXEDPRIMITIVE)Hooked1_DrawIndexedPrimitive;
			pDeviceData->pDeviceFuncs->pfnSetStreamSource = (PFND3DDDI_SETSTREAMSOURCE)Hooked1_SetStreamSource;
			pDeviceData->pDeviceFuncs->pfnCreateQuery = (PFND3DDDI_CREATEQUERY)Hooked1_CreateQuery;
			pDeviceData->pDeviceFuncs->pfnPresent = (PFND3DDDI_PRESENT)Hooked1_Present;
		}
	}

	return result;
}

static HRESULT APIENTRY Hooked_OpenAdapter(D3DDDIARG_OPENADAPTER *pAdapterData)
{
	HRESULT result = oOpenAdapter(pAdapterData);
	if (result == S_OK && pAdapterData->pAdapterFuncs->pfnCreateDevice != nullptr)
	{
		oCreateDevice = pAdapterData->pAdapterFuncs->pfnCreateDevice;
		pAdapterData->pAdapterFuncs->pfnCreateDevice = Hooked_CreateDevice;

		printf_s("oCreateDevice = 0x%X\n", (DWORD)oCreateDevice);
	}

	return result;
}

void InitGPUD3DHook()
{
	FnOpenAdapter* OpenAdapter = nullptr;

	do
	{
		HMODULE module = nullptr;
		if ((module = GetModuleHandleA("nvd3dum.dll")) != nullptr)
			OpenAdapter = (FnOpenAdapter*)GetProcAddress(module, "OpenAdapter");
		else if ((module = GetModuleHandleA("aticfx32.dll")) != nullptr)
			OpenAdapter = (FnOpenAdapter*)GetProcAddress(module, "OpenAdapter");

		if (OpenAdapter != nullptr)
			oOpenAdapter = DetourFunction(OpenAdapter, Hooked_OpenAdapter, 9);

	} while (OpenAdapter == nullptr);

	printf_s("oOpenAdapter = 0x%X\n", (DWORD)oOpenAdapter);
	
	// 创建一个线程来进行事情
	// CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)&StartStrideChange, NULL, NULL, NULL);
}

bool InitDirectX(HWND window)
{
	D3DPRESENT_PARAMETERS pp;
	HRESULT result = D3DERR_INVALIDCALL;
	ZeroMemory(&pp, sizeof(pp));
	pp.Windowed = TRUE;
	pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	pp.BackBufferFormat = D3DFMT_UNKNOWN;

	gD3D9Internal = Direct3DCreate9(D3D_SDK_VERSION);
	if (gD3D9Internal != nullptr)
	{
		result = gD3D9Internal->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING, &pp, &gDeviceInternal);
		// 如果方法成功，返回值为D3D_OK。如果方法失败，返回值可以是以下之一：D3DERR_DEVICELOST，D3DERR_INVALIDCALL，D3DERR_NOTAVAILABLE，D3DERR_OUTOFVIDEOMEMORY。
		if (result == D3DERR_DEVICELOST)
			printf("失败：Device 已丢失。\n");
		else if(result == D3DERR_INVALIDCALL)
			printf("失败：参数不正确。\n");
		else if(result == D3DERR_NOTAVAILABLE)
			printf("失败：这个对象不支持查询技术。\n");
		else if(result == D3DERR_OUTOFVIDEOMEMORY)
			printf("失败：内存不足。\n");
	}
	
	return (result == S_OK && gDeviceInternal != nullptr);
}

bool InitDesktopDirectX(HWND window)
{
	if (gIsInitialization)
		return false;

	return (gIsInitialization = InitDirectX(window));
}

static HINSTANCE gFakeInstance;
static LRESULT CALLBACK FakeWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

bool InitFakeDirectXDevice()
{
	WNDCLASSEXA wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW|CS_VREDRAW;
	wc.lpfnWndProc = FakeWindowProc;
	wc.hInstance = gFakeInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = "FakeWindow";
	RegisterClassExA(&wc);

	gFakeWindow = CreateWindowExA(NULL, "FakeWindow", "Fake DirectX Window", WS_OVERLAPPEDWINDOW,
		300, 300, 1024, 768, NULL, NULL, gFakeInstance, NULL);
	if (!gFakeWindow)
	{
		UnregisterClassA("FakeWindow", gFakeInstance);
		return false;
	}

	if (!InitDesktopDirectX(gFakeWindow))
	{
		gD3D9Internal->Release();
		CloseWindow(gFakeWindow);
		UnregisterClassA("FakeWindow", gFakeInstance);
		return false;
	}
	
	SetWindowLongA(gFakeWindow, GWL_STYLE, GetWindowLongA(gFakeWindow, GWL_STYLE)|WS_EX_NOPARENTNOTIFY);
	return true;
}

bool ReleaseFakeDirectXDevice()
{
	if (gDeviceInternal)
	{
		gDeviceInternal->Release();
		gDeviceInternal = nullptr;
	}

	if (gD3D9Internal)
	{
		gD3D9Internal->Release();
		gD3D9Internal = nullptr;
	}

	if (gFakeWindow)
	{
		DestroyWindow(gFakeWindow);
		UnregisterClassA("FakeWindow", gFakeInstance);
		gFakeWindow = nullptr;
	}
	
	return true;
}

static HANDLE CreateSurfaceforWDDM1_3(HANDLE hDevice, UINT width, UINT height)
{
	D3DDDIARG_CREATERESOURCE createResourceData2;
	D3DDDI_SURFACEINFO surfList[1];

	memset(surfList, 0, sizeof(surfList));
	memset(&createResourceData2, 0, sizeof(createResourceData2));

	surfList[0].Height = width;
	surfList[0].Width = height;
	surfList[0].Depth = 1;
	surfList[0].pSysMem = NULL;
	surfList[0].SysMemPitch = 0;
	surfList[0].SysMemSlicePitch = 0;

	createResourceData2.Format = D3DDDIFMT_A8R8G8B8;
	createResourceData2.Pool = D3DDDIPOOL_NONLOCALVIDMEM;
	createResourceData2.MultisampleType = D3DDDIMULTISAMPLE_NONE;
	createResourceData2.MultisampleQuality = 0;
	createResourceData2.pSurfList = surfList;
	createResourceData2.SurfCount = 1;
	createResourceData2.MipLevels = 0;
	createResourceData2.Fvf = 0;
	createResourceData2.VidPnSourceId = 0;
	createResourceData2.RefreshRate.Denominator = 0;
	createResourceData2.RefreshRate.Numerator = 0;
	// use a particular handle that runtime do not use
	createResourceData2.hResource = (HANDLE)0xfff00000;
	createResourceData2.Flags.Value = 0;
	createResourceData2.Rotation = D3DDDI_ROTATION_IDENTITY;

	oCreateResource(hDevice, &createResourceData2);

	return createResourceData2.hResource;
}

static HRESULT BltToRTWDDM1_1(HANDLE pDevice, HANDLE hResource, D3DDDIARG_PRESENT * pPresent)
{
	D3DDDIARG_BLT bltData;
	RECT srcRC;
	RECT destRC;

	memset(&bltData, 0, sizeof(bltData));
	memset(&srcRC, 0, sizeof(srcRC));
	memset(&destRC, 0, sizeof(destRC));

	srcRC.left = 0;
	srcRC.top = 0;
	srcRC.right = 200;
	srcRC.bottom = 200;

	destRC.left = 0;
	destRC.top = 0;
	destRC.right = 200;
	destRC.bottom = 200;

	bltData.hDstResource = pPresent->hSrcResource;
	bltData.DstSubResourceIndex = pPresent->SrcSubResourceIndex;
	bltData.hSrcResource = hResource;
	bltData.SrcSubResourceIndex = 0;
	bltData.SrcRect = srcRC;
	bltData.DstRect = destRC;

	return oBlt(pDevice, &bltData);
}

static HRESULT SurfaceBltInternal(HANDLE hDevice, HANDLE hDestSurf, RECT destSurfRC, IDirect3DSurface9 * pSrcSurf, RECT srcSurfRC)
{
	D3DDDIARG_LOCK lockdata;
	D3DDDIARG_UNLOCK unlockData;
	D3DLOCKED_RECT lockedRC;
	HRESULT hr;

	if (destSurfRC.right - destSurfRC.left < srcSurfRC.right - srcSurfRC.left ||
		destSurfRC.bottom - destSurfRC.top < srcSurfRC.bottom - srcSurfRC.top)
	{
		hr = S_FALSE;
	}
	else
	{
		memset(&lockdata, 0, sizeof(lockdata));
		memset(&unlockData, 0, sizeof(unlockData));

		lockdata.hResource = hDestSurf;
		lockdata.SubResourceIndex = 0;
		lockdata.Area.left = 0;
		lockdata.Area.right = destSurfRC.right - destSurfRC.left;
		lockdata.Area.top = 0;
		lockdata.Area.bottom = destSurfRC.bottom - destSurfRC.top;

		hr = oLock(hDevice, &lockdata);
		if (hr == S_OK)
		{
			hr = pSrcSurf->LockRect(&lockedRC, &srcSurfRC, NULL);
			if (S_OK == hr)
			{
				CHAR *pDest = (CHAR*)lockdata.pSurfData;
				CHAR *pSrc = (CHAR*)lockedRC.pBits;
				for (UINT i = 0; i < destSurfRC.bottom - destSurfRC.top; i++)
				{

					CopyMemory(pDest, pSrc, lockedRC.Pitch < lockdata.Pitch ? lockedRC.Pitch : lockdata.Pitch);
					pDest += lockdata.Pitch;
					pSrc += lockedRC.Pitch;
				}

			}
			pSrcSurf->UnlockRect();
			memset(&unlockData, 0, sizeof(unlockData));
			unlockData.hResource = hDestSurf;
			unlockData.SubResourceIndex = 0;
			oUnlock(hDevice, &unlockData);
		}
	}
	return hr;
}

static HRESULT APIENTRY OnRenderFrame(__out IDirect3DSurface9** pOutSurface, RECT* pOutRect)
{
	HRESULT result;
	RECT rect;

	gDeviceInternal->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	if (gDeviceInternal->BeginScene() == S_OK)
	{
		// result = gDeviceInternal->SetRenderTarget(NULL, );

		gDeviceInternal->EndScene();
	}

	if (result == S_OK)
	{
		// *pOutSurface = 
		*pOutRect = rect;
	}
	else
	{
		*pOutSurface = nullptr;
		pOutRect->top = pOutRect->left = pOutRect->bottom = pOutRect->right = 0;
	}

	return result;
}

static HRESULT APIENTRY Hooked_SetStreamSource(HANDLE pDevice, D3DDDIARG_SETSTREAMSOURCE* pSetStreamSource)
{
	if (pSetStreamSource->Stream == 0)
		gDetourFunc.iStride = pSetStreamSource->Stride;

	return gDetourFunc.oSetStreamSource(pDevice, pSetStreamSource);
}

static HRESULT APIENTRY Hooked_CreateQuery(HANDLE pDevice, D3DDDIARG_CREATEQUERY* pCreateQuery)
{
	if (pCreateQuery->QueryType == D3DDDIQUERYTYPE_OCCLUSION)
		pCreateQuery->QueryType = D3DDDIQUERYTYPE_TIMESTAMP;

	return gDetourFunc.oCreateQuery(pDevice, pCreateQuery);
}

static HRESULT APIENTRY Hooked_DrawIndexedPrimitive(HANDLE pDevice, D3DDDIARG_DRAWINDEXEDPRIMITIVE* pDrawIndexedPrimitive)
{
	if (l4d2_special(gDetourFunc1.iStride, pDrawIndexedPrimitive->NumVertices, pDrawIndexedPrimitive->PrimitiveCount))
	{
		D3DDDIARG_RENDERSTATE render;
		render.State = D3DDDIRS_ZENABLE;
		render.Value = 0;
		gDetourFunc.oSetRenderState(pDevice, &render);
		gDetourFunc.oDrawIndexedPrimitive(pDevice, pDrawIndexedPrimitive);

		render.State = D3DDDIRS_ZENABLE;
		render.Value = 1;	
		gDetourFunc.oSetRenderState(pDevice, &render);
	}

	return gDetourFunc.oDrawIndexedPrimitive(pDevice, pDrawIndexedPrimitive);
}

static HRESULT APIENTRY Hooked_Present(HANDLE pDevice, D3DDDIARG_PRESENT* pPresent)
{
	/*
	static HANDLE myDevice = nullptr, dstSurface = nullptr;
	static UINT width, height;
	static HRESULT result;

	if (pDevice == myDevice || myDevice == nullptr)
	{
		myDevice = pDevice;

		RECT rect;
		IDirect3DSurface9* surface;
		if (OnRenderFrame(&surface, &rect) == S_OK)
		{
			if (rect.right - rect.left != width || rect.bottom - rect.top != height)
			{
				width = rect.right - rect.left;
				height = rect.bottom - rect.top;

				// 记得要销毁这个对象
				dstSurface = CreateSurfaceforWDDM1_3(pDevice, width, height);
			}

			if (dstSurface != nullptr)
			{
				IDirect3DSurface9* src = nullptr;
				result = SurfaceBltInternal(pDevice, dstSurface, rect, src, rect);
			}

			if (result == S_OK)
				BltToRTWDDM1_1(pDevice, dstSurface, pPresent);
		}
	}

	result = gDetourFunc.oPresent(pDevice, pPresent);
	return result;
	*/

	return gDetourFunc.oPresent(pDevice, pPresent);
}

static HRESULT APIENTRY Hooked1_SetStreamSource(HANDLE pDevice, D3DDDIARG_SETSTREAMSOURCE* pSetStreamSource)
{
	if (pSetStreamSource->Stream == 0)
		gDetourFunc1.iStride = pSetStreamSource->Stride;

	return gDetourFunc1.oSetStreamSource(pDevice, pSetStreamSource);
}

static HRESULT APIENTRY Hooked1_CreateQuery(HANDLE pDevice, D3DDDIARG_CREATEQUERY* pCreateQuery)
{
	if (pCreateQuery->QueryType == D3DDDIQUERYTYPE_OCCLUSION)
		pCreateQuery->QueryType = D3DDDIQUERYTYPE_TIMESTAMP;

	return gDetourFunc1.oCreateQuery(pDevice, pCreateQuery);
}

static HRESULT APIENTRY Hooked1_DrawIndexedPrimitive(HANDLE pDevice, D3DDDIARG_DRAWINDEXEDPRIMITIVE* pDrawIndexedPrimitive)
{
	if (l4d2_special(gDetourFunc1.iStride, pDrawIndexedPrimitive->NumVertices, pDrawIndexedPrimitive->PrimitiveCount))
	{
		D3DDDIARG_RENDERSTATE render;
		render.State = D3DDDIRS_ZENABLE;
		render.Value = 0;
		gDetourFunc1.oSetRenderState(pDevice, &render);
		gDetourFunc1.oDrawIndexedPrimitive(pDevice, pDrawIndexedPrimitive);

		render.State = D3DDDIRS_ZENABLE;
		render.Value = 1;
		gDetourFunc1.oSetRenderState(pDevice, &render);
	}

	return gDetourFunc1.oDrawIndexedPrimitive(pDevice, pDrawIndexedPrimitive);
}

static HRESULT APIENTRY Hooked1_Present(HANDLE pDevice, D3DDDIARG_PRESENT* pPresent)
{
	return gDetourFunc1.oPresent(pDevice, pPresent);
}

DWORD BruteForceDevice(DWORD* pTable)
{
	DWORD dwResult, dwRounded;

	for (DWORD i = 0; i < 0x7FFFFFFF; i += 0x4)
	{
		__try
		{
			if (*(DWORD*)i == (DWORD)pTable)
			{
				for (int k = 0; k < 42; k++)
				{
					if (*(DWORD*)(*(DWORD*)i + k * 4) == *(pTable + k) && k == 41)
					{
						dwResult = i;
						i = 0x7FFFFFFF;
						break;
					}
				}
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			dwRounded = (i + 128) / 256 * 256;
			dwRounded += 256 - 4;
			i = dwRounded;
		}
	}

	return dwResult;
}

void* GetDirectXPointerFromMemory(void* pvReplica, DWORD dwVTable, DWORD dwAddress, DWORD dwSize)
{
	// don't know if it change something but the goal is avoid to do  (dwAddress + dwSize) at each iteration
	DWORD limit = (dwAddress + dwSize);

	for (DWORD* i = (DWORD*)dwAddress; (DWORD)i < limit; i++)
	{
		// I don't anticipate this will be an issue, if it is one use BYTE*
		__try
		{
			DWORD dwValue = (DWORD)&i;
			DWORD dwVTableAddress = *(DWORD*)dwValue;

			// We don't want to catch the pointer WE created
			if (dwValue == (DWORD)pvReplica)
				continue;

			if (dwVTableAddress == dwVTable)
			{
				printf("VTable Match (0x%X, 0x%X, 0x%X)\n", (DWORD)i, (DWORD)dwValue, (DWORD)dwVTableAddress);
				return (IDirect3DDevice9*)dwValue;
			}
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			/* No other way to check for valid pointers */
		}
	}

	return NULL;
}

void* FindHeapAddressWithVTable(void* pvReplica, DWORD dwVTable)
{
	printf("d3d9.dll = 0x%X\n", GetModuleHandleA("d3d9.dll"));

	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPHEAPLIST, GetCurrentProcessId());

	HEAPLIST32 lphl = { 0 };
	lphl.dwSize = sizeof(HEAPLIST32);

	if (Heap32ListFirst(hSnap, &lphl))
	{
		do
		{
			HEAPENTRY32 he = { 0 };
			he.dwSize = sizeof(HEAPENTRY32);
			if (Heap32First(&he, lphl.th32ProcessID, lphl.th32HeapID))
			{
				do
				{
					void* ptr = GetDirectXPointerFromMemory(pvReplica, dwVTable, he.dwAddress, he.dwBlockSize);
					if (ptr)
						return ptr;
				}
				while (Heap32Next(&he));
			}

		} while (Heap32ListNext(hSnap, &lphl));
	}

	printf("Failed.\n");
	return NULL;
}

bool CreateSearchDevice(IDirect3D9** d3d, IDirect3DDevice9** device)
{
	if (!d3d || !device)
		return false;

	*d3d = NULL;
	*device = NULL;

	IDirect3D9* pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	if (!pD3D)
		return false;

	HWND hWindow = GetDesktopWindow();

	D3DPRESENT_PARAMETERS pp;
	ZeroMemory(&pp, sizeof(pp));
	pp.Windowed = TRUE;
	pp.hDeviceWindow = hWindow;
	pp.BackBufferCount = 1;
	pp.BackBufferWidth = 4;
	pp.BackBufferHeight = 4;
	pp.BackBufferFormat = D3DFMT_X8R8G8B8;
	pp.SwapEffect = D3DSWAPEFFECT_DISCARD;

	IDirect3DDevice9* pDevice = NULL;

	if (SUCCEEDED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWindow,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, &pp, &pDevice)))
	{
		if (pDevice != NULL)
		{
			*d3d = pD3D;
			*device = pDevice;
		}
	}

	bool returnSuccess = (*d3d != NULL);

	if (returnSuccess == FALSE)
	{
		if (pD3D)
			pD3D->Release();
	}

	return returnSuccess;
}

IDirect3DDevice9* FindDirexcXDevice()
{
	while (!CreateSearchDevice(&gD3D9Internal, &gDeviceInternal))
	{
		printf("创建搜索用的 Device 失败。");
		Sleep(1000);
	}

	printf("Created Search Device! (0x%X, 0x%X)\n", (DWORD)gD3D9Internal, (DWORD)gDeviceInternal);

	IDirect3DDevice9* pDevice = nullptr;
	do
	{
		pDevice = (IDirect3DDevice9*)FindHeapAddressWithVTable(gDeviceInternal, *(DWORD*)gDeviceInternal);
		Sleep(1000);
	} while (pDevice == nullptr);

	printf("Found Device (0x%X)\n", (DWORD)pDevice);

	if (gD3D9Internal)
		gD3D9Internal->Release();
	if (gDeviceInternal)
		gDeviceInternal->Release();

	return pDevice;
}

NAMESPACE_END

template<typename Fn>
Fn * DetourFunction(Fn * src, Fn * dst, int len)
{
	/*
	static auto func = [](BYTE* src, BYTE* dst, int len) -> void*
	{
	BYTE *jmp = (BYTE*)VirtualAlloc(0, len + 5, MEM_COMMIT, 64);
	//BYTE *jmp = (BYTE*)malloc(len+5);
	DWORD dwBack;

	VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &dwBack);
	memcpy(jmp, src, len);
	jmp += len;
	jmp[0] = 0xE9;
	*(DWORD*)(jmp + 1) = (DWORD)(src + len - jmp) - 5;
	src[0] = 0xE9;
	*(DWORD*)(src + 1) = (DWORD)(dst - src) - 5;
	for (int i = 5; i<len; i++)  src[i] = 0x90;
	VirtualProtect(src, len, dwBack, &dwBack);
	return (jmp - len);
	};
	*/

	Fn* result = nullptr;
#if defined(EASYHOOK_API)
	HOOK_TRACE_INFO hti = { 0 };
	if (FAILED(LhInstallHook(src, dst, NULL, &hti)))
		return nullptr;

#elif defined(DETOURS_VERSION)
	DetourRestoreAfterWith();
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	result = (Fn*)DetourAttach((PVOID*)&src, dst);
	DetourTransactionCommit();
#else
	BYTE *jmp = (BYTE*)VirtualAlloc(0, len + 5, MEM_COMMIT, 64);
	// BYTE *jmp = (BYTE*)malloc(len+5);
	DWORD dwBack;

	VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &dwBack);
	memcpy(jmp, src, len);
	jmp += len;
	jmp[0] = 0xE9;
	*(DWORD*)(jmp + 1) = (DWORD)((BYTE*)src + len - jmp) - 5;
	((BYTE*)src)[0] = 0xE9;
	*(DWORD*)((BYTE*)src + 1) = (DWORD)((BYTE*)dst - (BYTE*)src) - 5;

	for (int i = 5; i < len; i++)
		((BYTE*)src)[i] = 0x90;

	VirtualProtect(src, len, dwBack, &dwBack);
	return (Fn*)(jmp - len);
#endif

	/*
	Fn* result = nullptr;
	#ifdef DETOURS_VERSION
	#if DETOURS_VERSION >= 21000
	DetourRestoreAfterWith();
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	result = (Fn*)DetourAttach((PVOID*)&src, dst);
	DetourTransactionCommit();
	#else
	result = (Fn*)DetourFunction((BYTE*)src, (BYTE*)dst);
	#endif

	return result;
	#else
	return (Fn*)func((BYTE*)src, (BYTE*)dst, len);
	#endif
	*/

#if defined(EASYHOOK_API) || defined(DETOURS_VERSION)
	return result;
#endif
}

template<typename Fn>
Fn* RetourFunction(Fn* src, Fn* dst, int len)
{
	DWORD dwback;

	if (!VirtualProtect(src, len, PAGE_READWRITE, &dwback))
		return false;

	if (!memcpy(src, dst, len))
		return false;

	dst[0] = 0xE9;
	*(DWORD*)((BYTE*)dst + 1) = (DWORD)((BYTE*)src - (BYTE*)dst) - 5;

	if (!VirtualProtect(src, len, dwback, &dwback))
		return false;

	return true;
}
