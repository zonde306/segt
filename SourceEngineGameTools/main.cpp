#include "main.h"
#include <mutex>

#define USE_PLAYER_INFO
#define USE_CVAR_CHANGE

extern LRESULT ImGui_ImplDX9_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

typedef void(__cdecl* FnConColorMsg)(class Color const&, char const*, ...);
static FnConColorMsg PrintToConsoleColor;

typedef void(__cdecl* FnConMsg)(char const*, ...);
static FnConMsg PrintToConsole;
std::ofstream errlog;

void StartCheat(HINSTANCE instance);

typedef HRESULT(WINAPI* FnDrawIndexedPrimitive)(IDirect3DDevice9*, D3DPRIMITIVETYPE, INT, UINT, UINT, UINT, UINT);
HRESULT WINAPI Hooked_DrawIndexedPrimitive(IDirect3DDevice9*, D3DPRIMITIVETYPE, INT, UINT, UINT, UINT, UINT);
static FnDrawIndexedPrimitive oDrawIndexedPrimitive;

typedef HRESULT(WINAPI* FnEndScene)(IDirect3DDevice9*);
HRESULT WINAPI Hooked_EndScene(IDirect3DDevice9*);
static FnEndScene oEndScene;

typedef HRESULT(WINAPI* FnCreateQuery)(IDirect3DDevice9*, D3DQUERYTYPE, IDirect3DQuery9**);
HRESULT WINAPI Hooked_CreateQuery(IDirect3DDevice9*, D3DQUERYTYPE, IDirect3DQuery9**);
static FnCreateQuery oCreateQuery;

typedef HRESULT(WINAPI* FnReset)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
HRESULT WINAPI Hooked_Reset(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
static FnReset oReset;

typedef HRESULT(WINAPI* FnPresent)(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*);
HRESULT WINAPI Hooked_Present(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*);
static FnPresent oPresent;

#ifdef __DETOURXS_H
DetourXS dDrawIndexedPrimitive, dEndScene, dReset, dCreateQuery, dPresent;
#endif

HINSTANCE hModuleCheats;

BOOL WINAPI DllMain(HINSTANCE Instance, DWORD Reason, LPVOID Reserved)
{
	if (Reason == DLL_PROCESS_ATTACH)
	{
		AllocConsole();

		HWND hwnd = GetConsoleWindow();
		hModuleCheats = Instance;

		HMENU hMenu = GetSystemMenu(hwnd, FALSE);
		if (hMenu) DeleteMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);

		SetConsoleTitleA(XorStr("Console"));
		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);
		// printf("RUN\n");
		Interfaces.GetInterfaces();
		netVars = new CNetVars();

		errlog.open(XorStr("segt.log"), std::ofstream::out|std::ofstream::app);
		Utils::log(XorStr("========= cheats start ========="));

		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)StartCheat, Instance, NULL, NULL);
	}
	else if (Reason == DLL_PROCESS_DETACH)
	{
#ifdef __DETOURXS_H
		if (dReset.Created())
			dReset.Destroy();
		if (dEndScene.Created())
			dEndScene.Destroy();
		if (dDrawIndexedPrimitive.Created())
			dDrawIndexedPrimitive.Destroy();
		if (dCreateQuery.Created())
			dCreateQuery.Destroy();
		if (dPresent.Created())
			dPresent.Destroy();
#endif
		if (Interfaces.ClientModeHook)
			Interfaces.ClientModeHook->HookTable(false);
		if (Interfaces.ClientHook)
			Interfaces.ClientHook->HookTable(false);
		if (Interfaces.ModelRenderHook)
			Interfaces.ModelRenderHook->HookTable(false);
		if (Interfaces.PanelHook)
			Interfaces.PanelHook->HookTable(false);
		if (Interfaces.PredictionHook)
			Interfaces.PredictionHook->HookTable(false);
	}

	return TRUE;
}

typedef void(__thiscall* FnPaintTraverse)(void*, unsigned int, bool, bool);
void __fastcall Hooked_PaintTraverse(void*, void*, unsigned int, bool, bool);
static FnPaintTraverse oPaintTraverse;

typedef bool(__stdcall* FnCreateMoveShared)(float, CUserCmd*);
bool __stdcall Hooked_CreateMoveShared(float, CUserCmd*);
static FnCreateMoveShared oCreateMoveShared;

typedef void(__stdcall* FnCreateMove)(int, float, bool);
void __stdcall Hooked_CreateMove(int, float, bool);
static FnCreateMove oCreateMove;

typedef void(__stdcall* FnFrameStageNotify)(ClientFrameStage_t);
void __stdcall Hooked_FrameStageNotify(ClientFrameStage_t);
static FnFrameStageNotify oFrameStageNotify;

typedef int(__stdcall* FnInKeyEvent)(int, ButtonCode_t, const char *);
int __stdcall Hooked_InKeyEvent(int, ButtonCode_t, const char *);
static FnInKeyEvent oInKeyEvent;

typedef void(__stdcall* FnRunCommand)(CBaseEntity*, CUserCmd*, CMoveHelper*);
void __stdcall Hooked_RunCommand(CBaseEntity*, CUserCmd*, CMoveHelper*);
static FnRunCommand oRunCommand;

typedef void(__stdcall* FnDrawModel)(PVOID, PVOID, const ModelRenderInfo_t&, matrix3x4_t*);
void __stdcall Hooked_DrawModel(PVOID, PVOID, const ModelRenderInfo_t&, matrix3x4_t*);
static FnDrawModel oDrawModel;

static DrawManager* drawRender;
static CBaseEntity* pCurrentAiming, *pTriggerAiming;
static CVMTHookManager gDirectXHook;
static ConVar *cvar_sv_cheats, *cvar_r_drawothermodels, *cvar_cl_drawshadowtexture, *cvar_mat_fullbright,
	*cvar_sv_pure, *cvar_sv_consistency, *cvar_mp_gamemode, *cvar_c_thirdpersonshoulder;
static bool bImGuiInitialized = false, bBoxEsp = true, bTriggerBot = false, bAimBot = false, bBhop = true, bAutoFire = true;
static std::timed_mutex mAimbot;

void bunnyHop(void*);
void autoPistol();
void autoAim();
void esp();
void bindAlias(int);
void pure(void*);
void meleeAttack();
void thirdPerson();
void showSpectator();
void transparent();
void autoShot();

void StartCheat(HINSTANCE instance)
{
	// GetClientModeNormal B8 ? ? ? ? C3

	typedef void*(__stdcall* FnGetClientMode)();
	FnGetClientMode GetClientModeNormal = nullptr;
	DWORD size, address;
	address = Utils::GetModuleBase(XorStr("client.dll"), &size);

	if ((GetClientModeNormal = (FnGetClientMode)Utils::FindPattern(address, size, XorStr("B8 ? ? ? ? C3"))) != nullptr)
	{
		Interfaces.ClientMode = GetClientModeNormal();
		if (Interfaces.ClientMode)
		{
			Interfaces.ClientModeHook = new CVMTHookManager(Interfaces.ClientMode);
			// printo("ClientModePtr", Interfaces.ClientMode);
			Utils::log(XorStr("ClientModePointer = 0x%X"), (DWORD)Interfaces.ClientMode);
		}
	}
	else
	{
		Interfaces.ClientMode = *(void**)(address + IClientModePointer);
		if (Interfaces.ClientMode && (DWORD)Interfaces.ClientMode > address)
		{
			Interfaces.ClientModeHook = new CVMTHookManager((void*)(*(DWORD*)Interfaces.ClientMode));
			// printo("ClientModePtr", Interfaces.ClientMode);
			Utils::log(XorStr("ClientModePointer = 0x%X"), (DWORD)Interfaces.ClientMode);
		}
	}
	
	if (Interfaces.PanelHook && indexes::PaintTraverse > -1)
	{
		oPaintTraverse = (FnPaintTraverse)Interfaces.PanelHook->HookFunction(indexes::PaintTraverse, Hooked_PaintTraverse);
		Interfaces.PanelHook->HookTable(true);
		// printo("oPaintTraverse", oPaintTraverse);
		Utils::log(XorStr("oPaintTraverse = 0x%X"), (DWORD)oPaintTraverse);
	}

	if (Interfaces.ClientModeHook && indexes::SharedCreateMove > -1)
	{
		oCreateMoveShared = (FnCreateMoveShared)Interfaces.ClientModeHook->HookFunction(indexes::SharedCreateMove, Hooked_CreateMoveShared);
		Interfaces.ClientModeHook->HookTable(true);
		// printo("oCreateMoveShared", oCreateMoveShared);
		Utils::log(XorStr("oCreateMoveShared = 0x%X"), (DWORD)oCreateMoveShared);
	}

	if (Interfaces.ClientHook && indexes::CreateMove > -1)
	{
		oCreateMove = (FnCreateMove)Interfaces.ClientHook->HookFunction(indexes::CreateMove, Hooked_CreateMove);
		Interfaces.ClientHook->HookTable(true);
		// printo("oCreateMove", oCreateMove);
		Utils::log(XorStr("oCreateMove = 0x%X"), (DWORD)oCreateMove);
	}

	if (Interfaces.ClientHook && indexes::FrameStageNotify > -1)
	{
		oFrameStageNotify = (FnFrameStageNotify)Interfaces.ClientHook->HookFunction(indexes::FrameStageNotify, Hooked_FrameStageNotify);
		Interfaces.ClientHook->HookTable(true);
		// printo("oFrameStageNotify", oFrameStageNotify);
		Utils::log(XorStr("oFrameStageNotify = 0x%X"), (DWORD)oFrameStageNotify);
	}

	if (Interfaces.ClientHook && indexes::InKeyEvent > -1)
	{
		oInKeyEvent = (FnInKeyEvent)Interfaces.ClientHook->HookFunction(indexes::InKeyEvent, Hooked_InKeyEvent);
		Interfaces.ClientHook->HookTable(true);
		// printo("oInKeyEvent", oInKeyEvent);
		Utils::log(XorStr("oInKeyEvent = 0x%X"), (DWORD)oInKeyEvent);
	}

	if (Interfaces.PredictionHook && indexes::RunCommand > -1)
	{
		oRunCommand = (FnRunCommand)Interfaces.ClientHook->HookFunction(indexes::RunCommand, Hooked_RunCommand);
		Interfaces.ClientHook->HookTable(true);
		// printo("oRunCommand", oRunCommand);
		Utils::log(XorStr("oRunCommand = 0x%X"), (DWORD)oRunCommand);
	}

	if (Interfaces.ModelRenderHook && indexes::DrawModel > -1)
	{
		oDrawModel = (FnDrawModel)Interfaces.ClientHook->HookFunction(indexes::DrawModel, Hooked_DrawModel);
		Interfaces.ClientHook->HookTable(true);
		// printo("oDrawModel", oDrawModel);
		Utils::log(XorStr("oDrawModel = 0x%X"), (DWORD)oDrawModel);
	}

	HMODULE tier0 = Utils::GetModuleHandleSafe("tier0.dll");
	if (tier0 != NULL)
	{
		PrintToConsole = (FnConMsg)GetProcAddress(tier0, "ConMsg");
		PrintToConsoleColor = (FnConColorMsg)GetProcAddress(tier0, "ConColorMsg");
		Utils::log(XorStr("PrintToConsole = 0x%X"), (DWORD)PrintToConsole);
		Utils::log(XorStr("PrintToConsoleColor = 0x%X"), (DWORD)PrintToConsoleColor);
	}

	if (Interfaces.Cvar)
	{
		cvar_sv_cheats = Interfaces.Cvar->FindVar(XorStr("sv_cheats"));
		cvar_r_drawothermodels = Interfaces.Cvar->FindVar(XorStr("r_drawothermodels"));
		cvar_cl_drawshadowtexture = Interfaces.Cvar->FindVar(XorStr("cl_drawshadowtexture"));
		cvar_mat_fullbright = Interfaces.Cvar->FindVar(XorStr("mat_fullbright"));
		cvar_sv_pure = Interfaces.Cvar->FindVar(XorStr("sv_pure"));
		cvar_sv_consistency = Interfaces.Cvar->FindVar(XorStr("sv_consistency"));
		cvar_mp_gamemode = Interfaces.Cvar->FindVar(XorStr("mp_gamemode"));
		cvar_c_thirdpersonshoulder = Interfaces.Cvar->FindVar(XorStr("c_thirdpersonshoulder"));
		
		/*
		printo("sv_cheats", cvar_sv_cheats);
		printo("r_drawothermodels", cvar_r_drawothermodels);
		printo("cl_drawshadowtexture", cvar_cl_drawshadowtexture);
		printo("mat_fullbright", cvar_mat_fullbright);
		printo("sv_pure", cvar_sv_pure);
		printo("sv_consistency", cvar_sv_consistency);
		printo("mp_gamemode", cvar_mp_gamemode);
		printo("c_thirdpersonshoulder", cvar_c_thirdpersonshoulder);
		*/

		Utils::log(XorStr("*** ConVar ***"));
		Utils::log(XorStr("sv_cheats = 0x%X"), (DWORD)cvar_sv_cheats);
		Utils::log(XorStr("r_drawothermodels = 0x%X"), (DWORD)cvar_r_drawothermodels);
		Utils::log(XorStr("cl_drawshadowtexture = 0x%X"), (DWORD)cvar_cl_drawshadowtexture);
		Utils::log(XorStr("mat_fullbright = 0x%X"), (DWORD)cvar_mat_fullbright);
		Utils::log(XorStr("sv_pure = 0x%X"), (DWORD)cvar_sv_pure);
		Utils::log(XorStr("sv_consistency = 0x%X"), (DWORD)cvar_sv_consistency);
		Utils::log(XorStr("mp_gamemode = 0x%X"), (DWORD)cvar_mp_gamemode);
		Utils::log(XorStr("c_thirdpersonshoulder = 0x%X"), (DWORD)cvar_c_thirdpersonshoulder);
		Utils::log(XorStr("*** end ***"));
	}

	dh::StartDeviceHook([&](IDirect3D9*& pD3D, IDirect3DDevice9*& pDevice, DWORD*& pVMT) -> void
	{
#ifdef __DETOURXS_H
		dReset.Create((void*)pVMT[16], Hooked_Reset);
		oReset = (FnReset)dReset.GetTrampoline();
		dEndScene.Create((void*)pVMT[42], Hooked_EndScene);
		oEndScene = (FnEndScene)dEndScene.GetTrampoline();
		dDrawIndexedPrimitive.Create((void*)pVMT[82], Hooked_DrawIndexedPrimitive);
		oDrawIndexedPrimitive = (FnDrawIndexedPrimitive)dDrawIndexedPrimitive.GetTrampoline();
		dCreateQuery.Create((void*)pVMT[118], Hooked_CreateQuery);
		oCreateQuery = (FnCreateQuery)dCreateQuery.GetTrampoline();
		dPresent.Create((void*)pVMT[17], Hooked_Present);
		oPresent = (FnPresent)dPresent.GetTrampoline();

#elif defined(DETOURS_VERSION)
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
#endif
		/*
		printo("oReset", oReset);
		printo("oEndScene", oEndScene);
		printo("oDrawIndexedPrimitive", oDrawIndexedPrimitive);
		printo("oCreateQuery", oCreateQuery);
		printo("oPresent", oPresent);
		*/

		Utils::log(XorStr("oReset = 0x%X"), (DWORD)oReset);
		Utils::log(XorStr("oPresent = 0x%X"), (DWORD)oPresent);
		Utils::log(XorStr("oEndScene = 0x%X"), (DWORD)oEndScene);
		Utils::log(XorStr("oDrawIndexedPrimitive = 0x%X"), (DWORD)oDrawIndexedPrimitive);
		Utils::log(XorStr("oCreateQuery = 0x%X"), (DWORD)oCreateQuery);
	});

	DWORD client, engine, material, fmWait;
	client = Utils::GetModuleBase(XorStr("client.dll"));
	engine = Utils::GetModuleBase(XorStr("engine.dll"));
	material = Utils::GetModuleBase(XorStr("materialsystem.dll"));

	Utils::log(XorStr("client.dll = 0x%X"), client);
	Utils::log(XorStr("engine.dll = 0x%X"), engine);
	Utils::log(XorStr("materialsystem.dll = 0x%X"), material);
	Utils::log(XorStr("localPlayer = 0x%X"), (DWORD)GetLocalClient());

	/*
	printo("client.dll", client);
	printo("engine.dll", engine);
	printo("materialsystem.dll", material);
	printo("localPlayer", (DWORD)GetLocalClient());
	*/

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)autoShot, NULL, NULL, NULL);
	// CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)pure, (void*)engine, NULL, NULL);
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)bunnyHop, (void*)client, NULL, NULL);
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)autoPistol, NULL, NULL, NULL);
	// CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)autoAim, NULL, NULL, NULL);

	fmWait = 45;
	bindAlias(fmWait);

	for (;;)
	{
		static bool connected = false;

		if (Interfaces.Engine->IsConnected())
		{
			if (GetAsyncKeyState(VK_INSERT) & 0x01)
			{
#ifdef USE_CVAR_CHANGE
				if (cvar_r_drawothermodels != nullptr)
				{
					if (cvar_r_drawothermodels->IsFlagSet(FCVAR_CHEAT | FCVAR_SPONLY | FCVAR_REPLICATED | FCVAR_NOT_CONNECTED | FCVAR_SERVER_CAN_EXECUTE))
						cvar_r_drawothermodels->RemoveFlags(FCVAR_CHEAT | FCVAR_SPONLY | FCVAR_REPLICATED | FCVAR_NOT_CONNECTED | FCVAR_SERVER_CAN_EXECUTE);

					if (!cvar_r_drawothermodels->IsFlagSet(FCVAR_SERVER_CANNOT_QUERY))
						cvar_r_drawothermodels->AddFlags(FCVAR_SERVER_CANNOT_QUERY);
				}

				if (cvar_cl_drawshadowtexture != nullptr)
				{
					if (cvar_cl_drawshadowtexture->IsFlagSet(FCVAR_CHEAT | FCVAR_SPONLY | FCVAR_REPLICATED | FCVAR_NOT_CONNECTED | FCVAR_SERVER_CAN_EXECUTE))
						cvar_cl_drawshadowtexture->RemoveFlags(FCVAR_CHEAT | FCVAR_SPONLY | FCVAR_REPLICATED | FCVAR_NOT_CONNECTED | FCVAR_SERVER_CAN_EXECUTE);

					if (!cvar_cl_drawshadowtexture->IsFlagSet(FCVAR_SERVER_CANNOT_QUERY))
						cvar_cl_drawshadowtexture->AddFlags(FCVAR_SERVER_CANNOT_QUERY);
				}
#endif
				
#ifdef USE_CVAR_CHANGE
				if (cvar_r_drawothermodels != nullptr && cvar_cl_drawshadowtexture != nullptr)
				{
					if (cvar_r_drawothermodels->GetInt() == 2)
					{
						cvar_r_drawothermodels->SetValue(1);
						cvar_cl_drawshadowtexture->SetValue(0);
					}
					else
					{
						cvar_r_drawothermodels->SetValue(2);
						cvar_cl_drawshadowtexture->SetValue(1);
					}

					Interfaces.Engine->ClientCmd(XorStr("echo \"[ConVar] r_drawothermodels set %d\""),
						cvar_r_drawothermodels->GetInt());
					Interfaces.Engine->ClientCmd(XorStr("echo \"[ConVar] cvar_cl_drawshadowtexture set %d\""),
						cvar_cl_drawshadowtexture->GetInt());
				}
				else
				{
#endif
					if (Utils::readMemory<int>(client + r_drawothermodels) == 2)
					{
						Utils::writeMemory(1, client + r_drawothermodels);
						Utils::writeMemory(0, client + cl_drawshadowtexture);
					}
					else
					{
						Utils::writeMemory(2, client + r_drawothermodels);
						Utils::writeMemory(1, client + cl_drawshadowtexture);
					}

					Interfaces.Engine->ClientCmd(XorStr("echo \"r_drawothermodels set %d\""),
						Utils::readMemory<int>(client + r_drawothermodels));
					Interfaces.Engine->ClientCmd(XorStr("echo \"cl_drawshadowtexture set %d\""),
						Utils::readMemory<int>(client + cl_drawshadowtexture));
#ifdef USE_CVAR_CHANGE
				}
#endif
				bBoxEsp = !bBoxEsp;
				Sleep(1000);
			}

			if (GetAsyncKeyState(VK_HOME) & 0x01)
			{
#ifdef USE_CVAR_CHANGE
				if (cvar_mat_fullbright != nullptr)
				{
					if (cvar_mat_fullbright->IsFlagSet(FCVAR_CHEAT | FCVAR_SPONLY | FCVAR_REPLICATED | FCVAR_NOT_CONNECTED | FCVAR_SERVER_CAN_EXECUTE))
						cvar_mat_fullbright->RemoveFlags(FCVAR_CHEAT | FCVAR_SPONLY | FCVAR_REPLICATED | FCVAR_NOT_CONNECTED | FCVAR_SERVER_CAN_EXECUTE);

					if (!cvar_mat_fullbright->IsFlagSet(FCVAR_SERVER_CANNOT_QUERY))
						cvar_mat_fullbright->AddFlags(FCVAR_SERVER_CANNOT_QUERY);
				}
#endif
				
#ifdef USE_CVAR_CHANGE
				if (cvar_mat_fullbright != nullptr)
				{
					if (cvar_mat_fullbright->GetInt() == 1)
						cvar_mat_fullbright->SetValue(0);
					else
						cvar_mat_fullbright->SetValue(1);

					Interfaces.Engine->ClientCmd(XorStr("echo \"[ConVar] mat_fullbright set %d\""),
						cvar_mat_fullbright->GetInt());
				}
				else
				{
#endif
					if (Utils::readMemory<int>(material + mat_fullbright) == 1)
						Utils::writeMemory(0, material + mat_fullbright);
					else
						Utils::writeMemory(1, material + mat_fullbright);

					Interfaces.Engine->ClientCmd(XorStr("echo \"mat_fullbright set %d\""),
						Utils::readMemory<int>(material + mat_fullbright));
#ifdef USE_CVAR_CHANGE
				}
#endif

				Sleep(1000);
			}

			if (GetAsyncKeyState(VK_PRIOR) & 0x01)
			{
#ifdef USE_CVAR_CHANGE
				if (cvar_mp_gamemode != nullptr)
				{
					if (cvar_mp_gamemode->IsFlagSet(FCVAR_CHEAT | FCVAR_SPONLY | FCVAR_REPLICATED | FCVAR_NOT_CONNECTED | FCVAR_SERVER_CAN_EXECUTE))
						cvar_mp_gamemode->RemoveFlags(FCVAR_CHEAT | FCVAR_SPONLY | FCVAR_REPLICATED | FCVAR_NOT_CONNECTED | FCVAR_SERVER_CAN_EXECUTE);

					if (!cvar_mp_gamemode->IsFlagSet(FCVAR_SERVER_CANNOT_QUERY))
						cvar_mp_gamemode->AddFlags(FCVAR_SERVER_CANNOT_QUERY);
				}
#endif
				
#ifdef USE_CVAR_CHANGE
				if (cvar_mp_gamemode != nullptr)
				{
					const char* mode = cvar_mp_gamemode->GetString();
					if (_strcmpi(mode, XorStr("versus")) == 0 || _strcmpi(mode, XorStr("realismversus")) == 0)
					{
						cvar_mp_gamemode->SetValue(XorStr("coop"));
						strcpy_s(cvar_mp_gamemode->m_pszString, cvar_mp_gamemode->m_StringLength, "coop");
					}
					else
					{
						cvar_mp_gamemode->SetValue(XorStr("versus"));
						strcpy_s(cvar_mp_gamemode->m_pszString, cvar_mp_gamemode->m_StringLength, XorStr("versus"));
					}

					Interfaces.Engine->ClientCmd(XorStr("echo \"[ConVar] mp_gamemode set %s\""),
						cvar_mp_gamemode->GetString());
				}
				else
				{
#endif
					char* mode = Utils::readMemory<char*>(client + mp_gamemode);
					if (mode != nullptr)
					{
						DWORD oldProtect = NULL;

						if (VirtualProtect(mode, sizeof(char) * 16, PAGE_EXECUTE_READWRITE, &oldProtect) == TRUE)
						{
							if (_strcmpi(mode, XorStr("versus")) == 0 || _strcmpi(mode, XorStr("realismversus")) == 0)
								strcpy_s(mode, 16, XorStr("coop"));
							else
								strcpy_s(mode, 16, XorStr("versus"));
							VirtualProtect(mode, sizeof(char) * 16, oldProtect, &oldProtect);
						}
						else
							printf("VirtualProtect 0x%X Fail!\n", (DWORD)mode);

						Interfaces.Engine->ClientCmd(XorStr("echo \"mp_gamemode set %s\""), mode);
					}
#ifdef USE_CVAR_CHANGE
				}
#endif

				Sleep(1000);
			}

			if (GetAsyncKeyState(VK_APPS) & 0x01)
			{
#ifdef USE_CVAR_CHANGE
				if (cvar_sv_cheats != nullptr)
				{
					// 解除修改限制
					if (cvar_sv_cheats->IsFlagSet(FCVAR_CHEAT | FCVAR_SPONLY | FCVAR_REPLICATED | FCVAR_NOT_CONNECTED | FCVAR_SERVER_CAN_EXECUTE))
						cvar_sv_cheats->RemoveFlags(FCVAR_CHEAT | FCVAR_SPONLY | FCVAR_REPLICATED | FCVAR_NOT_CONNECTED | FCVAR_SERVER_CAN_EXECUTE);

					// 防止被发现
					if (!cvar_sv_cheats->IsFlagSet(FCVAR_SERVER_CANNOT_QUERY))
						cvar_sv_cheats->AddFlags(FCVAR_SERVER_CANNOT_QUERY);
				}
#endif

#ifdef USE_CVAR_CHANGE
				if (cvar_sv_cheats != nullptr)
				{
					cvar_sv_cheats->SetValue(1);
					cvar_sv_cheats->m_fValue = 1.0f;
					cvar_sv_cheats->m_nValue = 1;
					Interfaces.Engine->ClientCmd(XorStr("echo \"[ConVar] sv_cheats set %d\""),
						cvar_sv_cheats->GetInt());
				}
#endif

				if (Utils::readMemory<int>(engine + sv_cheats) != 1)
				{
					Utils::writeMemory(1, engine + sv_cheats);
					Interfaces.Engine->ClientCmd(XorStr("echo \"sv_cheats set %d\""),
						Utils::readMemory<int>(engine + sv_cheats));
				}

				Sleep(1000);
			}

			if (GetAsyncKeyState(VK_NEXT) & 0x01)
				thirdPerson();

			if (GetAsyncKeyState(VK_CAPITAL) & 0x01)
				showSpectator();

			if (GetAsyncKeyState(VK_F9) & 0x01)
			{
				bTriggerBot = !bTriggerBot;
				Interfaces.Engine->ClientCmd(XorStr("echo \"[segt] trigger bot set %s\""),
					(bTriggerBot ? XorStr("enable") : XorStr("disabled")));

				Sleep(1000);
			}

			if (GetAsyncKeyState(VK_F10) & 0x01)
			{
				bAimBot = !bAimBot;
				Interfaces.Engine->ClientCmd(XorStr("echo \"[segt] aim bot set %s\""),
					(bAimBot ? XorStr("enable") : XorStr("disabled")));

				Sleep(1000);
			}

			if (GetAsyncKeyState(VK_F11) & 0x01)
			{
				bBhop = !bBhop;
				Interfaces.Engine->ClientCmd(XorStr("echo \"[segt] auto bunnyHop set %s\""),
					(bBhop ? XorStr("enable") : XorStr("disabled")));

				Sleep(1000);
			}

			if (GetAsyncKeyState(VK_F12) & 0x01)
			{
				bAutoFire = !bAutoFire;
				Interfaces.Engine->ClientCmd(XorStr("echo \"[segt] auto pistol fire %s\""),
					(bAutoFire ? XorStr("enable") : XorStr("disabled")));

				Sleep(1000);
			}

			if (!connected)
			{
				Interfaces.Engine->ClientCmd(XorStr("echo \"********* connected *********\""));
				cvar_sv_cheats = Interfaces.Cvar->FindVar(XorStr("sv_cheats"));
				cvar_r_drawothermodels = Interfaces.Cvar->FindVar(XorStr("r_drawothermodels"));
				cvar_cl_drawshadowtexture = Interfaces.Cvar->FindVar(XorStr("cl_drawshadowtexture"));
				cvar_mat_fullbright = Interfaces.Cvar->FindVar(XorStr("mat_fullbright"));
				cvar_sv_pure = Interfaces.Cvar->FindVar(XorStr("sv_pure"));
				cvar_sv_consistency = Interfaces.Cvar->FindVar(XorStr("sv_consistency"));
				cvar_mp_gamemode = Interfaces.Cvar->FindVar(XorStr("mp_gamemode"));
				cvar_c_thirdpersonshoulder = Interfaces.Cvar->FindVar(XorStr("c_thirdpersonshoulder"));

				Interfaces.Engine->ClientCmd(XorStr("echo \"sv_cheats = 0x%X\""), (DWORD)cvar_sv_cheats);
				Interfaces.Engine->ClientCmd(XorStr("echo \"r_drawothermodels = 0x%X\""), (DWORD)cvar_r_drawothermodels);
				Interfaces.Engine->ClientCmd(XorStr("echo \"r_drawothermodels = 0x%X\""), (DWORD)cvar_r_drawothermodels);
				Interfaces.Engine->ClientCmd(XorStr("echo \"mat_fullbright = 0x%X\""), (DWORD)cvar_mat_fullbright);
				Interfaces.Engine->ClientCmd(XorStr("echo \"sv_pure = 0x%X\""), (DWORD)cvar_sv_pure);
				Interfaces.Engine->ClientCmd(XorStr("echo \"sv_consistency = 0x%X\""), (DWORD)cvar_sv_consistency);
				Interfaces.Engine->ClientCmd(XorStr("echo \"mp_gamemode = 0x%X\""), (DWORD)cvar_mp_gamemode);
				Interfaces.Engine->ClientCmd(XorStr("echo \"c_thirdpersonshoulder = 0x%X\""), (DWORD)cvar_c_thirdpersonshoulder);

				logfile("connected");
			}

			connected = true;
		}
		else if (connected && !Interfaces.Engine->IsInGame())
		{
			static bool disconnected = false;
			disconnected = true;
			
#ifdef USE_CVAR_CHANGE
			if (cvar_r_drawothermodels != nullptr)
			{
				if (cvar_r_drawothermodels->GetInt() != 1)
				{
					cvar_r_drawothermodels->SetValue(1);
					disconnected = true;
				}
			}
			else
			{
#endif
				if (Utils::readMemory<int>(client + r_drawothermodels) != 1)
				{
					Utils::writeMemory<int>(1, client + r_drawothermodels);
					disconnected = true;
				}
#ifdef USE_CVAR_CHANGE
			}
#endif

#ifdef USE_CVAR_CHANGE
			if (cvar_cl_drawshadowtexture != nullptr)
			{
				if (cvar_cl_drawshadowtexture->GetInt() != 0)
				{
					cvar_cl_drawshadowtexture->SetValue(0);
					disconnected = true;
				}
			}
			else
			{
#endif
				if (Utils::readMemory<int>(client + cl_drawshadowtexture) != 0)
				{
					Utils::writeMemory(0, client + cl_drawshadowtexture);
					disconnected = true;
				}
#ifdef USE_CVAR_CHANGE
			}
#endif

#ifdef USE_CVAR_CHANGE
			if (cvar_mat_fullbright != nullptr)
			{
				if (cvar_mat_fullbright->GetInt() != 0)
				{
					cvar_mat_fullbright->SetValue(0);
					disconnected = true;
				}
			}
			else
			{
#endif
				if (Utils::readMemory<int>(material + mat_fullbright) != 0)
				{
					Utils::writeMemory(0, material + mat_fullbright);
					disconnected = true;
				}
#ifdef USE_CVAR_CHANGE
			}
#endif

			connected = false;
			if (disconnected)
			{
				disconnected = false;
				Interfaces.Engine->ClientCmd(XorStr("echo \"********* disconnected *********\""));
				logfile("disconnected");
				Sleep(9000);
			}
		}

		if (GetAsyncKeyState(VK_ADD) & 0x01)
		{
			Interfaces.Engine->ClientCmd(XorStr("alias fastmelee_loop \"+attack; slot1; wait 1; -attack; slot2; wait %d; fastmelee_launcher\""), ++fmWait);
			Interfaces.Engine->ClientCmd(XorStr("echo \"fastmelee wait set %d\""), fmWait);
			Sleep(1000);
		}

		if (GetAsyncKeyState(VK_SUBTRACT) & 0x01)
		{
			Interfaces.Engine->ClientCmd(XorStr("alias fastmelee_loop \"+attack; slot1; wait 1; -attack; slot2; wait %d; fastmelee_launcher\""), --fmWait);
			Interfaces.Engine->ClientCmd(XorStr("echo \"fastmelee wait set %d\""), fmWait);
			Sleep(1000);
		}

		if (Interfaces.Engine->IsConnected())
		{
#ifdef USE_CVAR_CHANGE
			if (cvar_sv_pure != nullptr)
			{
				if (cvar_sv_pure->IsFlagSet(FCVAR_CHEAT | FCVAR_SPONLY | FCVAR_REPLICATED | FCVAR_NOT_CONNECTED | FCVAR_SERVER_CAN_EXECUTE))
					cvar_sv_pure->RemoveFlags(FCVAR_CHEAT | FCVAR_SPONLY | FCVAR_REPLICATED | FCVAR_NOT_CONNECTED | FCVAR_SERVER_CAN_EXECUTE);

				if (!cvar_sv_pure->IsFlagSet(FCVAR_SERVER_CANNOT_QUERY))
					cvar_sv_pure->AddFlags(FCVAR_SERVER_CANNOT_QUERY);
			}
#endif

#ifdef USE_CVAR_CHANGE
			if (cvar_sv_consistency != nullptr)
			{
				if (cvar_sv_consistency->IsFlagSet(FCVAR_CHEAT | FCVAR_SPONLY | FCVAR_REPLICATED | FCVAR_NOT_CONNECTED | FCVAR_SERVER_CAN_EXECUTE))
					cvar_sv_consistency->RemoveFlags(FCVAR_CHEAT | FCVAR_SPONLY | FCVAR_REPLICATED | FCVAR_NOT_CONNECTED | FCVAR_SERVER_CAN_EXECUTE);

				if (!cvar_sv_consistency->IsFlagSet(FCVAR_SERVER_CANNOT_QUERY))
					cvar_sv_consistency->AddFlags(FCVAR_SERVER_CANNOT_QUERY);
			}
#endif

#ifdef USE_CVAR_CHANGE
			if (cvar_c_thirdpersonshoulder != nullptr)
			{
				if (cvar_c_thirdpersonshoulder->IsFlagSet(FCVAR_CHEAT | FCVAR_SPONLY | FCVAR_REPLICATED | FCVAR_NOT_CONNECTED | FCVAR_SERVER_CAN_EXECUTE))
					cvar_c_thirdpersonshoulder->RemoveFlags(FCVAR_CHEAT | FCVAR_SPONLY | FCVAR_REPLICATED | FCVAR_NOT_CONNECTED | FCVAR_SERVER_CAN_EXECUTE);

				// 防止被某些插件查询到
				if (!cvar_c_thirdpersonshoulder->IsFlagSet(FCVAR_SERVER_CANNOT_QUERY))
					cvar_c_thirdpersonshoulder->AddFlags(FCVAR_SERVER_CANNOT_QUERY);
			}
#endif

#ifdef USE_CVAR_CHANGE
			if (cvar_sv_pure != nullptr && cvar_sv_pure->GetInt() != 0)
				cvar_sv_pure->SetValue(0);
			if (cvar_sv_consistency != nullptr && cvar_sv_consistency->GetInt() != 0)
				cvar_sv_consistency->SetValue(0);
#endif

			if (Utils::readMemory<int>(engine + sv_pure) != 0 ||
				Utils::readMemory<int>(engine + sv_consistency) != 0)
			{
				Utils::writeMemory(0, engine + sv_pure);
				Utils::writeMemory(0, engine + sv_consistency);

				Interfaces.Engine->ClientCmd(XorStr("echo \"sv_pure and sv_consistency set %d\""),
					Utils::readMemory<int>(engine + sv_pure));

				Sleep(100);
			}
		}

		if (GetAsyncKeyState(VK_END) & 0x01)
		{
			errlog.close();
			ExitProcess(0);
		}

		if (GetAsyncKeyState(VK_DELETE) & 0x01)
			Interfaces.Engine->ClientCmd(XorStr("disconnect"));

		Sleep(1);
	}
}

bool IsAliveTarget(CBaseEntity* entity)
{
	if (entity == nullptr || entity->IsDormant())
		return false;

	int id = entity->GetClientClass()->m_ClassID;
	int solid = entity->GetNetProp<int>("m_usSolidFlags", "DT_BaseCombatCharacter");
	int sequence = entity->GetNetProp<int>("m_nSequence", "DT_BaseCombatCharacter");

	if (id == ET_TANK || id == ET_WITCH)
	{
		if ((solid & SF_NOT_SOLID) /*|| sequence > 70*/)
			return false;
	}
	else if (id == ET_BOOMER || id == ET_HUNTER || id == ET_SMOKER || id == ET_SPITTER ||
		id == ET_JOCKEY || id == ET_CHARGER)
	{
		if ((solid & SF_NOT_SOLID) || entity->GetHealth() < 1 ||
			entity->GetNetProp<int>("m_isGhost", "DT_TerrorPlayer") > 0)
			return false;
	}
	else if (id == ET_INFECTED)
	{
		if ((solid & SF_NOT_SOLID) /*|| sequence > 305*/)
			return false;
	}
	else if (id == ET_CTERRORPLAYER || id == ET_SURVIVORBOT)
	{
		if (!entity->IsAlive())
			return false;
	}
	else
	{
		static time_t next = 0;
		if (next < time(NULL))
			goto end_time_next;
		next = time(NULL) + 1;

		Utils::log(XorStr("Invalid ClassId = %d | sequence = %d | solid = %d"), id, sequence, solid);

	end_time_next:
		return false;
	}

	return true;
}

std::string GetZombieClassName(CBaseEntity* player)
{
	if (player == nullptr)
		return "";
	
	int zombie = player->GetNetProp<int>("m_zombieClass", "DT_TerrorPlayer");
	switch (zombie)
	{
	case ZC_SMOKER:
		return XorStr("smoker");
	case ZC_BOOMER:
		return XorStr("boomer");
	case ZC_HUNTER:
		return XorStr("hunter");
	case ZC_SPITTER:
		return XorStr("spitter");
	case ZC_JOCKEY:
		return XorStr("jockey");
	case ZC_CHARGER:
		return XorStr("charger");
	case ZC_TANK:
		return XorStr("tank");
	case ZC_SURVIVORBOT:
		return XorStr("survivor");
	}

	return "";
}

Vector GetHeadPosition(CBaseEntity* player)
{
	Vector position;
	int zombieClass = player->GetNetProp<int>("m_zombieClass", "DT_TerrorPlayer");
	if (zombieClass == ZC_SMOKER || zombieClass == ZC_HUNTER || zombieClass == ZC_TANK)
		position = player->GetBonePosition(BONE_NECK);
	else if (zombieClass == ZC_SPITTER || zombieClass == ZC_JOCKEY)
		position = player->GetBonePosition(BONE_JOCKEY_HEAD);
	else if (zombieClass == ZC_BOOMER)
		position = player->GetBonePosition(BONE_BOOMER_CHEST);
	else if (zombieClass == ZC_CHARGER)
		position = player->GetBonePosition(BONE_CHARGER_HEAD);
	else if (zombieClass == ZC_SURVIVORBOT)
	{
		int character = player->GetNetProp<int>("m_survivorCharacter", "DT_TerrorPlayer");
		switch (character)
		{
		case 0:
			position = player->GetBonePosition(BONE_NICK_HEAD);
			break;
		case 1:
			position = player->GetBonePosition(BONE_ROCHELLE_HEAD);
			break;
		case 2:
			position = player->GetBonePosition(BONE_COACH_HEAD);
			break;
		case 3:
			position = player->GetBonePosition(BONE_ELLIS_HEAD);
			break;
		case 4:
			position = player->GetBonePosition(BONE_BILL_HEAD);
			break;
		case 5:
			position = player->GetBonePosition(BONE_ZOEY_HEAD);
			break;
		case 6:
			position = player->GetBonePosition(BONE_FRANCIS_HEAD);
			break;
		case 7:
			position = player->GetBonePosition(BONE_LOUIS_HEAD);
			break;
		}
	}

	if (!position.IsValid() || position.IsZero(0.001f))
	{
		if (zombieClass == ZC_JOCKEY)
			position.z = pCurrentAiming->GetAbsOrigin().z + 30.0f;
		else if (zombieClass == ZC_HUNTER && (pCurrentAiming->GetFlags() & FL_DUCKING))
			position.z -= 12.0f;
	}

	return position;
}

CBaseEntity* GetAimingTarget(int hitbox = 0)
{
	CBaseEntity* client = GetLocalClient();
	if (client == nullptr || !client->IsAlive())
		return nullptr;

	trace_t trace;
	Vector src = client->GetEyePosition(), dst;
	Ray_t ray;

	CTraceFilter filter;
	filter.pSkip1 = client;
	Interfaces.Engine->GetViewAngles(dst);

	// angle (QAngle) to basic vectors.
	AngleVectors(dst, &dst);

	// multiply our angles by shooting range.
	dst *= 3500.f;

	// end point = our eye position + shooting range.
	dst += src;

	ray.Init(src, dst);

	/*
	Utils::log(XorStr("TraceRay: skip = 0x%X | start = (%.2f %.2f %.2f) | end = (%.2f %.2f %.2f)"),
		(DWORD)client, src.x, src.y, src.z, dst.x, dst.y, dst.z);
	*/

	Interfaces.Trace->TraceRay(ray, MASK_SHOT, &filter, &trace);

	/*
	Utils::log(XorStr("TraceRay: entity = 0x%X | hitbox = %d | bone = %d | hitGroup = %d | fraction = %.2f | classId = %d"),
		trace.m_pEnt, trace.hitbox, trace.physicsBone, trace.hitGroup, trace.fraction,
		(trace.m_pEnt != nullptr ? trace.m_pEnt->GetClientClass()->m_ClassID : -1));
	*/

	if (trace.m_pEnt != nullptr && !trace.m_pEnt->IsDormant() && trace.physicsBone > 0 &&
		trace.hitbox > 0 && (hitbox <= 0 || trace.hitbox == hitbox))
		return trace.m_pEnt;

	return nullptr;
}

bool IsTargetVisible(CBaseEntity* entity, const Vector& end = Vector())
{
	CBaseEntity* client = GetLocalClient();
	if (client == nullptr || !client->IsAlive() || entity == nullptr || !entity->IsAlive())
		return false;

	trace_t trace;
	Ray_t ray;

	CTraceFilter filter;
	filter.pSkip1 = client;

	if(end.IsValid())
		ray.Init(client->GetEyePosition(), end);
	else if(entity->GetClientClass()->m_ClassID == ET_INFECTED)
		ray.Init(client->GetEyePosition(), entity->GetHitboxPosition(HITBOX_COMMON));
	else
		ray.Init(client->GetEyePosition(), entity->GetHitboxPosition(HITBOX_PLAYER));

	Interfaces.Trace->TraceRay(ray, MASK_SHOT, &filter, &trace);
	if (trace.m_pEnt != nullptr && !trace.m_pEnt->IsDormant() && trace.hitbox > 0 && trace.physicsBone > 0)
		return true;

	return false;
}

void ResetDeviceHook(IDirect3DDevice9* device)
{
	dh::gDeviceInternal = device;
	if (dReset.Created())
		dReset.Destroy();
	if (dEndScene.Created())
		dEndScene.Destroy();
	if (dDrawIndexedPrimitive.Created())
		dDrawIndexedPrimitive.Destroy();
	if (dCreateQuery.Created())
		dCreateQuery.Destroy();
	if (dPresent.Created())
		dPresent.Destroy();

	gDirectXHook.Init(device);
	oReset = gDirectXHook.SetupHook(16, Hooked_Reset);
	oPresent = gDirectXHook.SetupHook(17, Hooked_Present);
	oEndScene = gDirectXHook.SetupHook(42, Hooked_EndScene);
	oDrawIndexedPrimitive = gDirectXHook.SetupHook(82, Hooked_DrawIndexedPrimitive);
	oCreateQuery = gDirectXHook.SetupHook(118, Hooked_CreateQuery);
	gDirectXHook.HookTable(true);

	std::cout << XorStr("========= Hook D3D Device =========") << std::endl;
	printv(dh::gDeviceInternal);
	printv(oReset);
	printv(oPresent);
	printv(oEndScene);
	printv(oDrawIndexedPrimitive);
	printv(oCreateQuery);
	std::cout << XorStr("========= Hook End =========") << std::endl;
}

void pure(void* engine)
{
	for (;;)
	{
		/*
		if (cvar_sv_pure != nullptr)
		{
			if (cvar_sv_pure->IsFlagSet(FCVAR_CHEAT | FCVAR_SPONLY | FCVAR_REPLICATED | FCVAR_NOT_CONNECTED | FCVAR_SERVER_CAN_EXECUTE))
				cvar_sv_pure->RemoveFlags(FCVAR_CHEAT | FCVAR_SPONLY | FCVAR_REPLICATED | FCVAR_NOT_CONNECTED | FCVAR_SERVER_CAN_EXECUTE);
		}

		if (cvar_sv_consistency != nullptr)
		{
			if (cvar_sv_consistency->IsFlagSet(FCVAR_CHEAT | FCVAR_SPONLY | FCVAR_REPLICATED | FCVAR_NOT_CONNECTED | FCVAR_SERVER_CAN_EXECUTE))
				cvar_sv_consistency->RemoveFlags(FCVAR_CHEAT | FCVAR_SPONLY | FCVAR_REPLICATED | FCVAR_NOT_CONNECTED | FCVAR_SERVER_CAN_EXECUTE);
		}
		*/
		
		if (cvar_sv_pure != nullptr && cvar_sv_pure->GetInt() != 0)
			cvar_sv_pure->SetValue(0);
		if (cvar_sv_consistency != nullptr && cvar_sv_consistency->GetInt() != 0)
			cvar_sv_consistency->SetValue(0);

		if (Utils::readMemory<int>((DWORD)engine + sv_pure) != 0 ||
			Utils::readMemory<int>((DWORD)engine + sv_consistency) != 0)
		{
			Utils::writeMemory(0, (DWORD)engine + sv_pure);
			Utils::writeMemory(0, (DWORD)engine + sv_consistency);

			Interfaces.Engine->ClientCmd(XorStr("echo \"sv_pure and sv_consistency set %d\""),
				Utils::readMemory<int>((DWORD)engine + sv_pure));
		}

		Sleep(100);
	}
}

void __stdcall Hooked_CreateMove(int sequence_number, float input_sample_frametime, bool active)
{
	static bool showHint = true;
	if (showHint)
	{
		showHint = false;
		std::cout << XorStr("Hooked_CreateMove trigged.") << std::endl;
		Utils::log(XorStr("Hooked_CreateMove success"));
	}
	
	oCreateMove(sequence_number, input_sample_frametime, active);

	/*
	DWORD dwEBP;
	__asm mov dwEBP, ebp
	PBYTE pSendPacket = (PBYTE)(*(PDWORD)(dwEBP) - 0x21);
	*/

	// 用不了，待修复
	// CVerifiedUserCmd* verified = &(*(CVerifiedUserCmd**)((DWORD)Interfaces.Input + 0xE0))[sequence_number % 150];
	// CUserCmd* cmd = &(*(CUserCmd**)((DWORD_PTR)Interfaces.Input + 0xDC))[sequence_number % 150];

	// CUserCmd* cmd = Interfaces.Input->GetUserCmd(sequence_number);
	
	CBaseEntity* client = GetLocalClient();
	if (client != nullptr && Interfaces.Engine->IsInGame() && !Interfaces.Engine->IsConsoleVisible() &&
		client->IsAlive() && mAimbot.try_lock())
	{
		// 自动瞄准
		if (bAimBot && GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		{
			Vector myOrigin = client->GetEyePosition(), myAngles = client->GetEyeAngles();

			// 寻找目标
			if (pCurrentAiming == nullptr || pCurrentAiming->IsDormant() || !IsAliveTarget(pCurrentAiming))
			{
				int team = client->GetTeam(), zombieClass;
				float distance = 32767.0f, dist, fov;
				bool visible = false;
				Vector headPos;

				pCurrentAiming = nullptr;
				for (int i = 1; i < 64; ++i)
				{
					CBaseEntity* target = Interfaces.ClientEntList->GetClientEntity(i);
					if (target == nullptr || target->IsDormant() || target->GetTeam() == team)
						continue;

					if (i < 64)
					{
						// 寻找一个玩家敌人
						if (!target->IsAlive() || target->GetHealth() <= 0)
							continue;

						zombieClass = target->GetNetProp<int>("m_zombieClass", "DT_TerrorPlayer");
						if (zombieClass < ZC_SMOKER || zombieClass > ZC_SURVIVORBOT || zombieClass == ZC_WITCH)
							continue;

						/*
						if (zombieClass == ZC_TANK && target->GetNetProp<int>("m_nSequence", "DT_BaseCombatCharacter") > 70)
							continue;
						*/

						// 选择最近的敌人
						try
						{
							// 通过 hitbox 可以准确的获得目标的头部位置
							headPos = target->GetHitboxPosition(HITBOX_PLAYER);

							// 检查目标是否可以被看见，看不见的就不能成为目标
							visible = IsTargetVisible(target, headPos);
						}
						catch (...)
						{
							headPos = target->GetEyePosition();
							if (zombieClass == ZC_JOCKEY)
								headPos.z = target->GetAbsOrigin().z + 30.0f;
							else if (zombieClass == ZC_HUNTER && (target->GetFlags() & FL_DUCKING))
								headPos.z -= 12.0f;

							visible = true;
						}

						dist = headPos.DistTo(myOrigin);
						fov = GetAnglesFieldOfView(myAngles, CalculateAim(myOrigin, headPos));
						if (visible && dist < distance && fov <= 30.f)
						{
							pCurrentAiming = target;
							distance = dist;
						}
					}
					else if (i > 64 && pCurrentAiming == nullptr && team == 2 && IsAliveTarget(target))
					{
						// 寻找一个普感敌人
						// 选择最近的敌人
						try
						{
							// 通过 hitbox 可以准确的获得目标的头部位置
							headPos = target->GetHitboxPosition(HITBOX_COMMON);

							// 检查目标是否可以被看见，看不见的就不能成为目标
							visible = IsTargetVisible(target, headPos);
						}
						catch (...)
						{
							continue;
						}

						dist = headPos.DistTo(myOrigin);
						fov = GetAnglesFieldOfView(myAngles, CalculateAim(myOrigin, headPos));
						if (visible && dist < distance && fov <= 30.f)
						{
							pCurrentAiming = target;
							distance = dist;
						}
					}
				}

				// 瞄准
				if (pCurrentAiming != nullptr)
				{
					// 目标位置
					Vector position;
					try
					{
						if(pCurrentAiming->GetClientClass()->m_ClassID == ET_INFECTED)
							position = pCurrentAiming->GetHitboxPosition(HITBOX_COMMON);
						else
							position = pCurrentAiming->GetHitboxPosition(HITBOX_PLAYER);
					}
					catch (...)
					{
						if (pCurrentAiming->GetClientClass()->m_ClassID == ET_INFECTED)
							goto end_aimbot;
						
						// 获取骨骼位置失败
						position = pCurrentAiming->GetEyePosition();
						Utils::log(XorStr("CBasePlayer::SetupBone error"));

						// 根据不同的情况确定高度
						int zombieClass = pCurrentAiming->GetNetProp<int>("m_zombieClass", "DT_TerrorPlayer");
						if (zombieClass == ZC_JOCKEY)
							position.z = pCurrentAiming->GetAbsOrigin().z + 30.0f;
						else if (zombieClass == ZC_HUNTER && (pCurrentAiming->GetFlags() & FL_DUCKING))
							position.z -= 12.0f;
					}

					// 速度预测，会导致屏幕晃动，所以不需要
					// position += (pCurrentAiming->GetVelocity() * Interfaces.Globals->interval_per_tick);

					Interfaces.Engine->SetViewAngles(CalculateAim(myOrigin, position));
				}
			}
		}
		
	end_aimbot:

		// 自动开枪
		if (bTriggerBot)
		{
			CBaseEntity* target = GetAimingTarget();
			if (target != nullptr && IsAliveTarget(target) && target->GetTeam() != client->GetTeam() &&
				(client->GetTeam() == 2 || target->GetClientClass()->m_ClassID != ET_INFECTED))
				pTriggerAiming = target;
			else if (pTriggerAiming != nullptr)
				pTriggerAiming = nullptr;
		}
		else if (pTriggerAiming != nullptr)
			pTriggerAiming = nullptr;

		mAimbot.unlock();
	}
}

bool __stdcall Hooked_CreateMoveShared(float flInputSampleTime, CUserCmd* cmd)
{
	static bool showHint = true;
	if (showHint)
	{
		showHint = false;
		std::cout << XorStr("Hooked_CreateMoveShared trigged.") << std::endl;
		Utils::log(XorStr("Hooked_CreateMoveShared success"));
	}
	
	CBaseEntity* client = GetLocalClient();

	if (!client || !cmd)
		return false;

	// 连跳
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		static bool lastJump = false;
		static bool shouldFake = false;

		if (!lastJump && shouldFake)
		{
			shouldFake = false;
			cmd->buttons |= IN_JUMP;
		}
		else if (cmd->buttons & IN_JUMP)
		{
			if (client->GetFlags() & FL_ONGROUND)
			{
				lastJump = true;
				shouldFake = true;
			}
			else
			{
				cmd->buttons &= ~IN_JUMP;
				lastJump = false;
			}
		}
		else
		{
			lastJump = false;
			shouldFake = false;
		}

		// strafe
		if (!(client->GetFlags() & FL_ONGROUND))
		{
			if (cmd->mousedx < 0)
				cmd->sidemove = -400.f;

			if (cmd->mousedx > 0)
				cmd->sidemove = 400.f;
		}
	}

	return oCreateMoveShared(flInputSampleTime, cmd);
}

void __fastcall Hooked_PaintTraverse(void* pPanel, void* edx, unsigned int panel, bool forcePaint, bool allowForce)
{
	static bool showHint = true;
	if (showHint)
	{
		showHint = false;
		std::cout << XorStr("Hooked_PaintTraverse trigged.") << std::endl;
		Utils::log(XorStr("Hooked_PaintTraverse success"));
	}
	
	static unsigned int MatSystemTopPanel = 0;
	static unsigned int FocusOverlayPanel = 0;

	if (MatSystemTopPanel == 0 || FocusOverlayPanel == 0)
	{
		const char* panelName = Interfaces.Panel->GetName(panel);
		if (panelName[0] == 'M' && panelName[3] == 'S' && panelName[9] == 'T')
		{
			MatSystemTopPanel = panel;
			Utils::log(XorStr("panel %s found %d"), panelName, MatSystemTopPanel);
		}
		else if (panelName[0] == 'F' && panelName[5] == 'O')
		{
			FocusOverlayPanel = panel;
			Utils::log(XorStr("panel %s found %d"), panelName, FocusOverlayPanel);
		}
	}

	if (FocusOverlayPanel > 0 && panel == FocusOverlayPanel)
	{
		
	}

	if (MatSystemTopPanel > 0 && panel == MatSystemTopPanel)
	{

	}

	// ((FnPaintTraverse)Interfaces.PanelHook->GetOriginalFunction(indexes::PaintTraverse))(ecx, panel, forcePaint, allowForce);
	oPaintTraverse(pPanel, panel, forcePaint, allowForce);
}

void __stdcall Hooked_FrameStageNotify(ClientFrameStage_t stage)
{
	static bool showHint = true;
	if (showHint)
	{
		showHint = false;
		std::cout << XorStr("Hooked_FrameStageNotify trigged.") << std::endl;
		Utils::log(XorStr("Hooked_FrameStageNotify success"));
	}
	
	QAngle punch;
	CBaseEntity* client = GetLocalClient();

	if (stage == FRAME_RENDER_START && Interfaces.Engine->IsInGame())
	{
		if (client != nullptr && client->IsAlive())
			punch = client->GetNetProp<Vector>("m_vecPunchAngle", "DT_BasePlayer");
	}

	oFrameStageNotify(stage);

	if (client != nullptr && client->IsAlive() && punch.IsValid())
		client->SetNetProp<Vector>("m_vecPunchAngle", punch, "DT_BasePlayer");
}

int __stdcall Hooked_InKeyEvent(int eventcode, ButtonCode_t keynum, const char *pszCurrentBinding)
{
	return oInKeyEvent(eventcode, keynum, pszCurrentBinding);
}

void __stdcall Hooked_RunCommand(CBaseEntity* pEntity, CUserCmd* pCmd, CMoveHelper* moveHelper)
{
	oRunCommand(pEntity, pCmd, moveHelper);

	Interfaces.MoveHelper = moveHelper;
}

void __stdcall Hooked_DrawModel(PVOID context, PVOID state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld)
{
	oDrawModel(context, state, pInfo, pCustomBoneToWorld);
}

HRESULT WINAPI Hooked_Reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pp)
{
	static bool showHint = true;
	if (showHint)
	{
		showHint = false;
		std::cout << XorStr("Hooked_Reset trigged.") << std::endl;

		if((DWORD)dh::gDeviceInternal == (DWORD)device)
			Utils::log(XorStr("Hooked_Reset success"));
	}

	if (dh::gDeviceInternal == nullptr)
	{
		ResetDeviceHook(device);
		showHint = true;
	}

	if(!bImGuiInitialized)
		return oReset(device, pp);

	// ImGui_ImplDX9_InvalidateDeviceObjects();
	drawRender->OnLostDevice();
	
	HRESULT result = oReset(device, pp);

	drawRender->OnResetDevice();
	// ImGui_ImplDX9_CreateDeviceObjects();
	
	return result;
}

HRESULT WINAPI Hooked_EndScene(IDirect3DDevice9* device)
{
	static bool showHint = true;
	if (showHint)
	{
		showHint = false;
		// printf("Hooked_EndScene trigged.");
		std::cout << XorStr("Hooked_EndScene trigged.") << std::endl;

		if ((DWORD)dh::gDeviceInternal == (DWORD)device)
			Utils::log(XorStr("Hooked_EndScene success"));
	}
	
	if (dh::gDeviceInternal == nullptr)
	{
		ResetDeviceHook(device);
		showHint = true;
	}

	if (!bImGuiInitialized)
	{
		// 初始化
		// ImGui_ImplDX9_Init(FindWindowA(NULL, "Left 4 Dead 2"), device);
		drawRender = new DrawManager(device);
		bImGuiInitialized = true;
	}
	else
	{
		// ImGui::GetIO().MouseDrawCursor = !Interfaces.Engine->IsInGame();
		
		// ImGui_ImplDX9_NewFrame();
		drawRender->BeginRendering();

		CBaseEntity* local = GetLocalClient();
		if(bBoxEsp)
		{
			static auto getPlayerColor = [](CBaseEntity* local, CBaseEntity* other) -> D3DCOLOR
			{
				static D3DCOLOR enemy = D3DCOLOR_RGBA(255, 0, 0, 255),
					team = D3DCOLOR_RGBA(0, 0, 255, 255);
				
				if (local->GetTeam() == other->GetTeam())
					return team;

				return enemy;
			};
			
			// int maxEntity = Interfaces.ClientEntList->GetHighestEntityIndex();
			for (int i = 1; i < 64; ++i)
			{
				CBaseEntity* entity = Interfaces.ClientEntList->GetClientEntity(i);
				if (entity == nullptr || entity->IsDormant() || (DWORD)entity == (DWORD)local)
					continue;

				if (i < 64)
				{
					// 玩家
					if (!entity->IsAlive() || entity->GetHealth() <= 0)
						continue;
					
					Vector head, foot;
					D3DCOLOR color = getPlayerColor(local, entity);

					if (WorldToScreen(entity->GetEyePosition(), head) &&
						WorldToScreen(entity->GetAbsOrigin(), foot))
					{
						// 显示类型
						drawRender->RenderText(color, head.x, head.y, true, "[%d] %s",
							entity->GetHealth(), GetZombieClassName(entity).c_str());

						static bool showHint = true;
						if (showHint)
						{
							Utils::log(XorStr("zombieClass Draw: %s"), GetZombieClassName(entity).c_str());
							showHint = false;
						}

						float height = fabs(head.y - foot.y);
						float width = height * 0.65f;

						// 绘制一个框
						drawRender->RenderRect(color, foot.x - width / 2, foot.y, width, -height);
						// drawRender->DrawRect(foot.x - width / 2, foot.y, width, -height, color);
					}
				}
				else
				{
					// 普感
					// TODO: 如果这个实体是普感，就绘制出来
				}
			}
		}

		// ImGui::Render();
		drawRender->EndRendering();
	}

	return oEndScene(device);
}

HRESULT WINAPI Hooked_DrawIndexedPrimitive(IDirect3DDevice9* device, D3DPRIMITIVETYPE type, INT baseIndex,
	UINT minIndex, UINT numVertices, UINT startIndex, UINT primitiveCount)
{
	static bool showHint = true;
	if (showHint)
	{
		showHint = false;
		// printf("Hooked_DrawIndexedPrimitive trigged.");
		// std::cout << XorStr("Hooked_DrawIndexedPrimitive trigged.") << std::endl;
		if ((DWORD)dh::gDeviceInternal == (DWORD)device)
			Utils::log(XorStr("Hooked_DrawIndexedPrimitive success"));
	}
	
	if (dh::gDeviceInternal == nullptr)
	{
		ResetDeviceHook(device);
		showHint = true;
	}

	if (Interfaces.Engine->IsInGame())
	{
		IDirect3DVertexBuffer9* stream = nullptr;
		UINT offsetByte, stride;
		device->GetStreamSource(0, &stream, &offsetByte, &stride);
		if (l4d2_special(stride, numVertices, primitiveCount) ||
			l4d2_weapons(stride, numVertices, primitiveCount) ||
			l4d2_stuff(stride, numVertices, primitiveCount) ||
			l4d2_survivor(stride, numVertices, primitiveCount))
		{
			static DWORD oldZEnable;
			device->GetRenderState(D3DRS_ZENABLE, &oldZEnable);
			device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
			device->SetRenderState(D3DRS_ZFUNC, D3DCMP_NEVER);
			oDrawIndexedPrimitive(device, type, baseIndex, minIndex, numVertices, startIndex, primitiveCount);
			device->SetRenderState(D3DRS_ZENABLE, oldZEnable);
			device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
			// oDrawIndexedPrimitive(device, type, baseIndex, minIndex, numVertices, startIndex, primitiveCount);
		}
	}

	return oDrawIndexedPrimitive(device, type, baseIndex, minIndex, numVertices, startIndex, primitiveCount);
}

HRESULT WINAPI Hooked_CreateQuery(IDirect3DDevice9* device, D3DQUERYTYPE type, IDirect3DQuery9** query)
{
	static bool showHint = true;
	if (showHint)
	{
		showHint = false;
		// printf("Hooked_CreateQuery trigged.");
		// std::cout << XorStr("Hooked_CreateQuery trigged.") << std::endl;
		if ((DWORD)dh::gDeviceInternal == (DWORD)device)
			Utils::log(XorStr("Hooked_CreateQuery success"));
	}
	
	if (dh::gDeviceInternal == nullptr)
	{
		ResetDeviceHook(device);
		showHint = true;
	}

	/*
	if (type == D3DQUERYTYPE_OCCLUSION)
		type = D3DQUERYTYPE_TIMESTAMP;
	*/

	return oCreateQuery(device, type, query);
}

HRESULT WINAPI Hooked_Present(IDirect3DDevice9* device, const RECT* source, const RECT* dest, HWND window, const RGNDATA* region)
{
	static bool showHint = true;
	if (showHint)
	{
		showHint = false;
		// printf("Hooked_Present trigged.");
		// std::cout << XorStr("Hooked_Present trigged.") << std::endl;
		if ((DWORD)dh::gDeviceInternal == (DWORD)device)
			Utils::log(XorStr("Hooked_Present success"));
	}
	
	if (dh::gDeviceInternal == nullptr)
	{
		ResetDeviceHook(device);
		showHint = true;
	}

	return oPresent(device, source, dest, window, region);
}

void bunnyHop(void* client)
{
	CBaseEntity* local = nullptr;
	
	for (;;)
	{
		local = GetLocalClient();
		if (bBhop && (GetAsyncKeyState(VK_SPACE) & 0x8000) && local != nullptr && local->IsAlive() &&
			Interfaces.Engine->IsInGame() && !Interfaces.Engine->IsConsoleVisible())
		{
			/*
			if (client->GetFlags() & FL_ONGROUND)
				Interfaces.Engine->ClientCmd("+jump");
			else
				Interfaces.Engine->ClientCmd("-jump");
			*/

			static bool repeat = false;
			int flags = local->GetFlags();
			if (flags != FL_CLIENT && flags != (FL_DUCKING | FL_CLIENT) && flags != (FL_INWATER | FL_CLIENT) &&
				flags != (FL_DUCKING | FL_CLIENT | FL_INWATER))
			{
				if (Utils::readMemory<int>((DWORD)client + m_iButtons) & IN_JUMP)
				{
					Interfaces.Engine->ClientCmd(XorStr("-jump"));
					Sleep(1);
				}

				Interfaces.Engine->ClientCmd(XorStr("+jump"));
				repeat = true;
			}
			else if (flags == FL_CLIENT || flags == (FL_DUCKING | FL_CLIENT) || flags == (FL_INWATER | FL_CLIENT) ||
				flags == (FL_DUCKING | FL_CLIENT | FL_INWATER))
			{
				Interfaces.Engine->ClientCmd(XorStr("-jump"));
				if (repeat)
				{
					Sleep(16);
					Interfaces.Engine->ClientCmd(XorStr("+jump"));
					Sleep(1);
					Interfaces.Engine->ClientCmd(XorStr("-jump"));
					repeat = false;
				}
			}
		}

		Sleep(1);
	}
}

void autoPistol()
{
	CBaseEntity* client = nullptr;
	
	for (;;)
	{
		client = GetLocalClient();
		if (bAutoFire && client && Interfaces.Engine->IsInGame() && !Interfaces.Engine->IsConsoleVisible() &&
			client->IsAlive() && (GetAsyncKeyState(VK_LBUTTON) & 0x8000))
		{
			CBaseEntity* weapon = (CBaseEntity*)client->GetActiveWeapon();
			if (weapon)
				weapon = Interfaces.ClientEntList->GetClientEntityFromHandle(weapon);

			if (weapon && weapon->GetNetProp<int>("m_iClip1", "DT_BaseCombatWeapon") > 0 &&
				weapon->GetNetProp<float>("m_flNextPrimaryAttack", "DT_BaseCombatWeapon") <= GetServerTime())
			{
				int weaponId = weapon->GetWeaponID();
				int aiming = *(int*)(client + m_iCrosshairsId);
				CBaseEntity* target = (aiming > 0 ? Interfaces.ClientEntList->GetClientEntity(aiming) : nullptr);
				if (target == nullptr || target->GetTeam() != client->GetTeam() || IsNeedRescue(target))
				{
					// 检查当前武器是否需要连点才能持续开枪
					// 因为这个连点开枪效率并不好，对于自动武器来说，会降低射击速度
					// 连发霰弹枪连点可以加快射速
					if (weaponId == Weapon_Pistol || weaponId == Weapon_ShotgunPump ||
						weaponId == Weapon_ShotgunAuto || weaponId == Weapon_SniperHunting ||
						weaponId == Weapon_ShotgunChrome || weaponId == Weapon_SniperMilitary ||
						weaponId == Weapon_ShotgunSpas || weaponId == Weapon_PistolMagnum ||
						weaponId == Weapon_SniperAWP || weaponId == Weapon_SniperScout)
					{
						Interfaces.Engine->ClientCmd(XorStr("+attack"));
						Sleep(10);
						Interfaces.Engine->ClientCmd(XorStr("-attack"));
					}
				}
				else
				{
					// 当前瞄准的是队友，并且队友不需要帮助
					// 在这里停止开枪，防止队友伤害
					Interfaces.Engine->ClientCmd(XorStr("-attack"));

					// printf("current weapon id = %d\n", weaponId);
				}
			}
		}

		Sleep(1);
	}
}

void autoAim()
{
	for (;;)
	{
		mAimbot.lock();
		
		CBaseEntity* client = GetLocalClient();
		if (client != nullptr && Interfaces.Engine->IsInGame() && client->IsAlive() &&
			!Interfaces.Engine->IsConsoleVisible())
		{
			CBaseEntity* weapon = (CBaseEntity*)client->GetActiveWeapon();
			if (weapon)
				weapon = Interfaces.ClientEntList->GetClientEntityFromHandle(weapon);

			if (weapon && (GetAsyncKeyState(VK_XBUTTON2) & 0x8000))
			{
				// 当前位置
				Vector myOrigin = client->GetEyePosition();

				// 检查是否需要选择新的敌人
				if (pCurrentAiming == nullptr || pCurrentAiming->IsDormant() ||
					!pCurrentAiming->IsAlive() || pCurrentAiming->GetHealth() <= 0)
				{
					Vector myAngles;
					Interfaces.Engine->GetViewAngles(myAngles);

					int team = client->GetTeam();
					float distance = 32767.0f, dist, fov;

					// int max = Interfaces.ClientEntList->GetHighestEntityIndex();
					for (int i = 1; i <= 64; ++i)
					{
						CBaseEntity* target = Interfaces.ClientEntList->GetClientEntity(i);
						if (target == nullptr /*|| target->IsDormant()*/ || target->GetTeam() == team ||
							!target->IsAlive() || target->GetHealth() <= 0)
							continue;

						/*
						int classId = (int)target->GetClientClass();
						if (classId != ET_BOOMER && classId != ET_JOCKEY && classId != ET_SPITTER &&
							classId != ET_CHARGER && classId != ET_HUNTER && classId != ET_SMOKER &&
							classId != ET_TANK && classId != ET_INFECTED)
							continue;
						*/

						int zombieClass = target->GetNetProp<int>("m_zombieClass", "DT_TerrorPlayer");
						if (zombieClass < ZC_SMOKER || zombieClass > ZC_SURVIVORBOT || zombieClass == ZC_WITCH)
							continue;

						// 选择最近的敌人
						dist = target->GetEyePosition().DistTo(myOrigin);
						fov = GetAnglesFieldOfView(myAngles, CalculateAim(myOrigin, target->GetEyePosition()));
						if (dist < distance && fov <= 30.f)
						{
							pCurrentAiming = target;
							distance = dist;
						}

						/*
						Vector angles = CalculateAim(client->GetEyePosition(), target->GetEyePosition());
						angles.z = 0.0f;

						client->GetEyeAngles() = angles;
						break;
						*/
					}
				}

				if (pCurrentAiming != nullptr)
				{
					// 目标位置
					Vector position;
					try
					{
						// 根据头部骨头来瞄准，这玩意很不稳定，时不时会 boom
						// 好处是瞄得准，不会被其他因数影响
						position = GetHeadPosition(pCurrentAiming);
					}
					catch(...)
					{
						// 获取骨骼位置失败
						position = pCurrentAiming->GetEyePosition();
						Utils::log(XorStr("CBasePlayer::SetupBone error"));

						// 根据不同的情况确定高度
						int zombieClass = pCurrentAiming->GetNetProp<int>("m_zombieClass", "DT_TerrorPlayer");
						if (zombieClass == ZC_JOCKEY)
							position.z = pCurrentAiming->GetAbsOrigin().z + 30.0f;
						else if (zombieClass == ZC_HUNTER && (pCurrentAiming->GetFlags() & FL_DUCKING))
							position.z -= 12.0f;
					}

					// 速度预测，会导致屏幕晃动，所以不需要
					// position += (pCurrentAiming->GetVelocity() * Interfaces.Globals->interval_per_tick);

					Interfaces.Engine->SetViewAngles(CalculateAim(myOrigin, position));
				}
			}
			else if (pCurrentAiming != nullptr)
			{
				pCurrentAiming = nullptr;
				Interfaces.Engine->ClientCmd(XorStr("echo \"*** auto aim stopped ***\""));
			}
		}

		mAimbot.unlock();
		Sleep(1);
	}
}

void esp()
{
	if (Interfaces.Engine->IsInGame())
	{
		for (int i = 1; i <= 64; ++i)
		{
			CBaseEntity* player = Interfaces.ClientEntList->GetClientEntity(i);
			if (player == nullptr || player->IsDormant() || !player->IsAlive() || player->GetHealth() <= 0)
				continue;

			// 没有任何效果...
			player->SetNetProp("m_iGlowType", 3, "DT_TerrorPlayer");
			// player->SetNetProp("m_nGlowRange", 0, "DT_GlowProperty");
			// player->SetNetProp("m_nGlowRangeMin", 0, "DT_GlowProperty");
			player->SetNetProp("m_glowColorOverride", 16777215, "DT_TerrorPlayer");
			// player->SetNetProp("m_bSurvivorGlowEnabled", 1, "DT_TerrorPlayer");

			/*
			CBaseEntity* glow = player->GetNetProp<CBaseEntity*>("m_Glow", "DT_BaseEntity");
			if (glow)
				glow = Interfaces.ClientEntList->GetClientEntityFromHandle(glow);
			if (glow)
			{
				glow->SetNetProp("m_iGlowType", 3, "DT_GlowProperty");
				// glow->SetNetProp("m_nGlowRange", 0, "DT_GlowProperty");
				// glow->SetNetProp("m_nGlowRangeMin", 0, "DT_GlowProperty");
				glow->SetNetProp("m_glowColorOverride", 16777215, "DT_GlowProperty");
			}
			*/
		}
		
		Sleep(1);
	}

	Sleep(1);
}

void meleeAttack()
{
	CBaseEntity* client = GetLocalClient();
	static float tookoutTime;

	if (client != nullptr && client->IsAlive())
	{
		CBaseEntity* weapon = (CBaseEntity*)client->GetActiveWeapon();
		if (weapon)
			weapon = Interfaces.ClientEntList->GetClientEntityFromHandle(weapon);


		float serverTime = GetServerTime();
		if (weapon && weapon->GetWeaponID() == Weapon_Melee && tookoutTime <= serverTime &&
			weapon->GetNetProp<float>("m_flNextPrimaryAttack", "DT_BaseCombatWeapon") <= serverTime)
		{
			Interfaces.Engine->ClientCmd(XorStr("+attack"));
			Sleep(10);
			Interfaces.Engine->ClientCmd(XorStr("-attack"));
			Interfaces.Engine->ClientCmd(XorStr("slot1"));
			Sleep(10);
			Interfaces.Engine->ClientCmd(XorStr("slot2"));
			tookoutTime = GetServerTime() + 0.5f;
		}
	}
	else
		tookoutTime = 0.0f;
	
	Sleep(1);
}

void thirdPerson()
{
	CBaseEntity* local = GetLocalClient();
	if (local && local->IsAlive())
	{
		if (local->GetNetProp<float>("m_TimeForceExternalView", "DT_TerrorPlayer") > 0.0f)
			local->SetNetProp<float>("m_TimeForceExternalView", 99999.3f, "DT_TerrorPlayer");
		else
			local->SetNetProp<float>("m_TimeForceExternalView", 0.0f, "DT_TerrorPlayer");
		
		/*
		if (local->GetNetProp<int>("m_hObserverTarget", "DT_BasePlayer") == -1)
		{
			// 切换到第三人称
			local->SetNetProp<int>("m_hObserverTarget", 0, "DT_BasePlayer");
			local->SetNetProp<int>("m_iObserverMode", 1, "DT_BasePlayer");
			local->SetNetProp<int>("m_bDrawViewmodel", 0, "DT_BasePlayer");
		}
		else if (local->GetNetProp<int>("m_hObserverTarget", "DT_BasePlayer") == 0)
		{
			// 切换到第一人称
			local->SetNetProp<int>("m_hObserverTarget", -1, "DT_BasePlayer");
			local->SetNetProp<int>("m_iObserverMode", 0, "DT_BasePlayer");
			local->SetNetProp<int>("m_bDrawViewmodel", 1, "DT_BasePlayer");
		}
		*/
	}

	Sleep(1000);
}

void showSpectator()
{
	CBaseEntity* local = GetLocalClient();
	if (local && local->IsAlive())
	{
#ifdef USE_PLAYER_INFO
		player_info_t info;
#endif

		CBaseEntity* player = nullptr, *target = nullptr;

		Interfaces.Engine->ClientCmd(XorStr("echo \"========= player list =========\""));

		for (int i = 1; i < 64; ++i)
		{
			player = Interfaces.ClientEntList->GetClientEntity(i);
			if (player == nullptr || player->IsDormant())
				continue;

			int team = player->GetTeam();
			int mode = player->GetNetProp<int>("m_hObserverTarget", "DT_BasePlayer");

#ifdef USE_PLAYER_INFO
			Interfaces.Engine->GetPlayerInfo(i, &info);
#endif

			if (team == 1)
			{
				// 观察者
				if (mode != 4 && mode != 5)
					continue;
				
				target = (CBaseEntity*)player->GetNetProp<void*>("m_iObserverMode", "DT_BasePlayer");
				if (target && (int)target != -1)
					target = Interfaces.ClientEntList->GetClientEntityFromHandle(target);
				
				if (target == nullptr || (int)target == -1)
					continue;
				
				if ((DWORD)target == (DWORD)local)
				{
					// 正在观察本地玩家
#ifdef USE_PLAYER_INFO
					Interfaces.Engine->ClientCmd(XorStr("echo \"[spectator] player %s %s you\""),
						info.name, (mode == 4 ? XorStr("watching") : XorStr("following")));
#else
					Interfaces.Engine->ClientCmd(XorStr("echo \"[spectator] player %d %s you\""),
						player->GetIndex(), (mode == 4 ? XorStr("watching") : XorStr("following")));
#endif
				}
				else
				{
					// 正在观察其他玩家
#ifdef USE_PLAYER_INFO
					player_info_t other;
					Interfaces.Engine->GetPlayerInfo(target->GetIndex(), &other);
					Interfaces.Engine->ClientCmd(XorStr("echo \"[spectator] player %s %s %s\""),
						info.name, (mode == 4 ? XorStr("watching") : XorStr("following")), other.name);
#else
					Interfaces.Engine->ClientCmd(XorStr("echo \"[spectator] player %d %s %d\""),
						player->GetIndex(), (mode == 4 ? XorStr("watching") : XorStr("following")), target->GetIndex());
#endif
				}
			}
			else if (team == 2)
			{
				// 生还者
				if (player->IsAlive())
				{
					// 活着
#ifdef USE_PLAYER_INFO
					Interfaces.Engine->ClientCmd(XorStr("echo \"[survivor] player %s is alive (%d + %.0f)\""),
						info.name, player->GetHealth(), player->GetNetProp<float>("m_healthBuffer", "DT_TerrorPlayer"));
#else
					Interfaces.Engine->ClientCmd(XorStr("echo \"[survivor] player %d is alive (%d + %.0f)\""),
						player->GetIndex(), player->GetHealth(), player->GetNetProp<float>("m_healthBuffer", "DT_TerrorPlayer"));
#endif
				}
				else
				{
					// 死亡
					if (mode != 4 && mode != 5)
						continue;
					
					target = (CBaseEntity*)player->GetNetProp<void*>("m_iObserverMode", "DT_BasePlayer");
					if (target && (int)target != -1)
						target = Interfaces.ClientEntList->GetClientEntityFromHandle(target);

					if (target == nullptr || (int)target == -1)
						continue;

					if ((DWORD)target == (DWORD)local)
					{
						// 正在观察本地玩家
#ifdef USE_PLAYER_INFO
						Interfaces.Engine->ClientCmd(XorStr("echo \"[survivor] player %s %s you\""),
							info.name, (mode == 4 ? XorStr("watching") : XorStr("following")));
#else
						Interfaces.Engine->ClientCmd(XorStr("echo \"[survivor] player %d %s you\""),
							player->GetIndex(), (mode == 4 ? XorStr("watching") : XorStr("following")));
#endif
					}
					else
					{
						// 正在观察其他玩家
#ifdef USE_PLAYER_INFO
						player_info_t other;
						Interfaces.Engine->GetPlayerInfo(target->GetIndex(), &other);
						Interfaces.Engine->ClientCmd(XorStr("echo \"[survivor] player %s %s %s\""),
							info.name, (mode == 4 ? XorStr("watching") : XorStr("following")), other.name);
#else
						Interfaces.Engine->ClientCmd(XorStr("echo \"[survivor] player %d %s %d\""),
							player->GetIndex(), (mode == 4 ? XorStr("watching") : XorStr("following")), target->GetIndex());
#endif
					}
				}
			}
			else if (team == 3)
			{
				// 感染者
				int zombie = player->GetNetProp<int>("m_zombieClass", "DT_TerrorPlayer");
				char zombieName[32];
				switch (zombie)
				{
				case ZC_SMOKER:
					strcpy_s(zombieName, XorStr("smoker"));
					break;
				case ZC_BOOMER:
					strcpy_s(zombieName, XorStr("boomer"));
					break;
				case ZC_HUNTER:
					strcpy_s(zombieName, XorStr("hunter"));
					break;
				case ZC_SPITTER:
					strcpy_s(zombieName, XorStr("spitter"));
					break;
				case ZC_JOCKEY:
					strcpy_s(zombieName, XorStr("jockey"));
					break;
				case ZC_CHARGER:
					strcpy_s(zombieName, XorStr("charger"));
					break;
				case ZC_TANK:
					strcpy_s(zombieName, XorStr("tank"));
					break;
				default:
					ZeroMemory(zombieName, 32);
				}

				if (player->IsAlive())
				{
					// 活着
#ifdef USE_PLAYER_INFO
					Interfaces.Engine->ClientCmd(XorStr("echo \"[infected] player %s is %s (%d)\""),
						info.name, zombieName, player->GetHealth());
#else
					Interfaces.Engine->ClientCmd(XorStr("echo \"[infected] player %d is %s (%d)\""),
						player->GetIndex(), zombieName, player->GetHealth());
#endif
				}
				else if (player->GetNetProp<int>("m_isGhost", "DT_TerrorPlayer"))
				{
					// 幽灵状态
#ifdef USE_PLAYER_INFO
					Interfaces.Engine->ClientCmd(XorStr("echo \"[infected] player %s is ghost %s (%d)\""),
						info.name, zombieName, player->GetHealth());
#else
					Interfaces.Engine->ClientCmd(XorStr("echo \"[infected] player %d is ghost %s (%d)\""),
						player->GetIndex(), zombieName, player->GetHealth());
#endif
				}
				else
				{
					// 死亡
					if (mode != 4 && mode != 5)
						continue;

					target = (CBaseEntity*)player->GetNetProp<void*>("m_iObserverMode", "DT_BasePlayer");
					if (target && (int)target != -1)
						target = Interfaces.ClientEntList->GetClientEntityFromHandle(target);

					if (target == nullptr || (int)target == -1)
						continue;

					if ((DWORD)target == (DWORD)local)
					{
						// 正在观察本地玩家
#ifdef USE_PLAYER_INFO
						Interfaces.Engine->ClientCmd(XorStr("echo \"[infected] player %s %s you\""),
							info.name, (mode == 4 ? XorStr("watching") : XorStr("following")));
#else
						Interfaces.Engine->ClientCmd(XorStr("echo \"[infected] player %s %s you\""),
							player->GetIndex(), (mode == 4 ? XorStr("watching") : XorStr("following")));
#endif
					}
					else
					{
						// 正在观察其他玩家
#ifdef USE_PLAYER_INFO
						player_info_t other;
						Interfaces.Engine->GetPlayerInfo(target->GetIndex(), &other);
						Interfaces.Engine->ClientCmd(XorStr("echo \"[infected] player %s %s %s\""),
							info.name, (mode == 4 ? XorStr("watching") : XorStr("following")), other.name);
#else
						Interfaces.Engine->ClientCmd(XorStr("echo \"[infected] player %d %s %d\""),
							player->GetIndex(), (mode == 4 ? XorStr("watching") : XorStr("following")), target->GetIndex());
#endif
					}
				}
			}
		}

		Interfaces.Engine->ClientCmd(XorStr("echo \"========= list end =========\""));
	}

	Sleep(1000);
}

void bindAlias(int wait)
{
	Interfaces.Engine->ClientCmd(XorStr("echo \"echo \"========= alias begin =========\"\""));
	Interfaces.Engine->ClientCmd(XorStr("alias +autofire \"alias autofire_launcher autofire_loop; autofire_launcher\""));
	Interfaces.Engine->ClientCmd(XorStr("alias -autofire \"alias autofire_launcher autofire_stop\""));
	Interfaces.Engine->ClientCmd(XorStr("alias autofire_launcher autofire_loop"));
	Interfaces.Engine->ClientCmd(XorStr("alias autofire_loop \"+attack; wait 5; -attack; wait 5; autofire_launcher\""));
	Interfaces.Engine->ClientCmd(XorStr("alias autofire_stop \"-attack\""));
	Interfaces.Engine->ClientCmd(XorStr("alias +autojump \"alias autojump_launcher autojump_loop; autojump_launcher\""));
	Interfaces.Engine->ClientCmd(XorStr("alias -autojump \"alias autojump_launcher autojump_stop\""));
	Interfaces.Engine->ClientCmd(XorStr("alias autojump_launcher autojump_loop"));
	Interfaces.Engine->ClientCmd(XorStr("alias autojump_loop \"+jump; wait 5; -jump; wait 5; autojump_launcher\""));
	Interfaces.Engine->ClientCmd(XorStr("alias autojump_stop \"-jump\""));
	Interfaces.Engine->ClientCmd(XorStr("alias +bigjump \"+jump; +duck\""));
	Interfaces.Engine->ClientCmd(XorStr("alias -bigjump \"-jump; -duck\""));
	Interfaces.Engine->ClientCmd(XorStr("alias +pistolspam \"alias pistolspam dopistolspam; dopistolspam\""));
	Interfaces.Engine->ClientCmd(XorStr("alias -pistolspam \"alias pistolspam -use\""));
	Interfaces.Engine->ClientCmd(XorStr("alias dopistolspam \"+use; wait 3; -use; wait 3; pistolspam\""));
	Interfaces.Engine->ClientCmd(XorStr("alias +fastmelee \"alias fastmelee_launcher fastmelee_loop; fastmelee_launcher\""));
	Interfaces.Engine->ClientCmd(XorStr("alias fastmelee_launcher fastmelee_loop"));
	Interfaces.Engine->ClientCmd(XorStr("alias fastmelee_loop \"+attack; slot1; wait 1; -attack; slot2; wait 45; fastmelee_launcher\""));
	Interfaces.Engine->ClientCmd(XorStr("alias fastmelee_stop \"-attack\""));
	Interfaces.Engine->ClientCmd(XorStr("alias -fastmelee \"alias fastmelee_launcher fastmelee_stop\""));
	Interfaces.Engine->ClientCmd(XorStr("alias thirdperson_toggle \"thirdperson_enable\""));
	Interfaces.Engine->ClientCmd(XorStr("alias thirdperson_enable \"alias thirdperson_toggle thirdperson_disable; thirdpersonshoulder\""));
	Interfaces.Engine->ClientCmd(XorStr("alias thirdperson_disable \"alias thirdperson_toggle thirdperson_enable; thirdpersonshoulder; c_thirdpersonshoulder 0\""));
	Interfaces.Engine->ClientCmd(XorStr("c_thirdpersonshoulderoffset 0"));
	Interfaces.Engine->ClientCmd(XorStr("c_thirdpersonshoulderaimdist 720"));
	Interfaces.Engine->ClientCmd(XorStr("cam_ideallag 0"));
	Interfaces.Engine->ClientCmd(XorStr("cam_idealdist 40"));
	Interfaces.Engine->ClientCmd(XorStr("bind leftarrow \"incrementvar cam_idealdist 30 130 10\""));
	Interfaces.Engine->ClientCmd(XorStr("bind rightarrow \"incrementvar cam_idealdist 30 130 -10\""));
	Interfaces.Engine->ClientCmd(XorStr("bind uparrow \"incrementvar c_thirdpersonshoulderheight 5 25 5\""));
	Interfaces.Engine->ClientCmd(XorStr("bind downarrow \"incrementvar c_thirdpersonshoulderheight 5 25 -5\""));
	Interfaces.Engine->ClientCmd(XorStr("echo \"echo \"========= alias end =========\"\""));
}

void transparent()
{
	CBaseEntity* local = GetLocalClient();
	if (local != nullptr && Interfaces.Engine->IsInGame() && local->IsAlive())
	{
		// local->SetNetProp("m_nRenderMode", RENDER_NONE);
		local->SetNetProp("m_CollisionGroup", CG_DEBRIS);
		// local->SetNetProp("m_nSolidType", (int)SOLID_NONE, "DT_CollisionProperty");
		// local->SetNetProp("m_usSolidFlags", SF_NOT_SOLID, "DT_CollisionProperty");
		local->SetNetProp("m_fEffects", local->GetNetProp<int>("m_fEffects") | EF_NODRAW);
		// local->SetNetProp("movetype", MOVETYPE_FLYGRAVITY);
	}

	Sleep(1);
}

void autoShot()
{
	CBaseEntity* local = nullptr;
	for (;;)
	{
		local = GetLocalClient();
		if (local != nullptr && Interfaces.Engine->IsInGame() && local->IsAlive())
		{
			CBaseEntity* weapon = (CBaseEntity*)local->GetActiveWeapon();
			if (weapon)
				local = Interfaces.ClientEntList->GetClientEntityFromHandle(weapon);

			if (weapon != nullptr && pTriggerAiming != nullptr)
			{
				Interfaces.Engine->ClientCmd(XorStr("+attack"));
				Sleep(9);
				Interfaces.Engine->ClientCmd(XorStr("-attack"));
			}
		}

		Sleep(1);
	}
}
