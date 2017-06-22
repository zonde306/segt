#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <d3d9types.h>
#include "d3dumddi.h"
#include <cstdio>
#include <functional>
#include <dwmapi.h>
#include "overlay.h"

// #include "../detours/detours.h"
// #include <easyhook.h>

#ifdef EASYHOOK_API
#if defined(_AMD64_) || defined(_IA64_) || defined(_ARM64_)
	#pragma comment(lib, "EasyHook64")
#else
	#pragma comment(lib, "EasyHook32")
#endif
#endif

#ifdef DETOURS_VERSION
#pragma comment(lib, "detours")
#else
#include "detourxs.h"
#endif

#ifdef D3D_SDK_VERSION
#pragma comment(lib, "d3d9")
#pragma comment(lib, "d3dx9")
#endif

#ifdef DWMAPI
#pragma comment(lib, "dwmapi")
#endif

#define NAMESPACE_START(_x)		namespace _x {
#define NAMESPACE_END			}

NAMESPACE_START(dh)

typedef HRESULT (APIENTRY FnOpenAdapter)(D3DDDIARG_OPENADAPTER*);
typedef struct D3DHook
{
	PFND3DDDI_DRAWINDEXEDPRIMITIVE oDrawIndexedPrimitive = nullptr;
	PFND3DDDI_SETRENDERSTATE oSetRenderState = nullptr;
	PFND3DDDI_CREATEQUERY oCreateQuery = nullptr;
	PFND3DDDI_SETSTREAMSOURCE oSetStreamSource = nullptr;
	PFND3DDDI_PRESENT oPresent = nullptr;
	PFND3DDDI_LOCK oLock = nullptr;
	PFND3DDDI_CREATERESOURCE oCreateResource = nullptr;
	PFND3DDDI_UNLOCK oUnlock = nullptr;
	PFND3DDDI_BLT oBlt = nullptr;

	DWORD iStride;
} D3DPFNHooker;

extern Overlay* gOverlay;
extern IDirect3D9* gD3D9Internal;
extern IDirect3DDevice9* gDeviceInternal;
extern FnOpenAdapter* oOpenAdapter;
extern PFND3DDDI_CREATEDEVICE oCreateDevice;
extern D3DPFNHooker gDetourFunc, gDetourFunc1;
extern PFND3DDDI_DRAWINDEXEDPRIMITIVE oDrawIndexedPrimitive;
extern PFND3DDDI_SETRENDERSTATE oSetRenderState;
extern PFND3DDDI_CREATEQUERY oCreateQuery;
extern PFND3DDDI_SETSTREAMSOURCE oSetStreamSource;
extern PFND3DDDI_PRESENT oPresent;
extern PFND3DDDI_LOCK oLock;
extern PFND3DDDI_CREATERESOURCE oCreateResource;
extern PFND3DDDI_UNLOCK oUnlock;
extern PFND3DDDI_BLT oBlt;

/*
* 初始化 D3D 钩子
*
* @return None
*/
void InitGPUD3DHook();

/*
* 尝试初始化 Direct3D9
*
* @return			如果成功返回 true 否则 false
*/
bool InitDesktopDirectX(HWND window = nullptr);

/*
* 创建一个假的窗口，用户 hook 原函数
*
* @return			如果成功返回 true 否则 false
*/
bool InitFakeDirectXDevice();

/*
* 将假窗口释放掉
*
* @return			如果成功返回 true 否则 false
*/
bool ReleaseFakeDirectXDevice();

/*
* 开始一个 IDirect3DDevice9 挂钩
* 如果成功，则会调用 func 作为 Hook 函数
*
* @param func		挂钩用的函数，其中 IDirect3D9* 为 pD3D， IDirect3DDevice9* 为 pD3DDevice，
*					DWORD* 为虚函数表指针，直接使用这个来作 Hook 用途
*
* @return			None
*/
void StartDeviceHook(std::function<void(IDirect3D9*&, IDirect3DDevice9*&, DWORD*&)> func);

// 尝试搜索 D3D Device 指针，但是非常慢
IDirect3DDevice9* FindDirexcXDevice();

/*
* 开始一个 IDirect3DDevice9 挂钩
* 如果成功，则会调用 func 作为 Hook 函数
*
* @param window		被覆盖的窗口
*
* @return			None
*/
void CreateOverlay(HWND window, HINSTANCE instance);

NAMESPACE_END

/*
* 挂钩函数
*
* @param src		原函数
* @param dst		你要修改到哪个函数
* @param len		函数大小
*
* @return			原函数的地址
*/
template<typename Fn>
Fn* DetourFunction(Fn* src, Fn* dst, int len);

/*
* 重新挂钩函数
*
* @param src		原函数
* @param dst		你要修改到哪个函数
* @param len		函数大小
*
* @return			原函数的地址
*/
template<typename Fn>
Fn* RetourFunction(Fn* src, Fn* dst, int len);
