#include "main.h"
#include <mutex>
#include <sstream>

// #define USE_PLAYER_INFO
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

		SetConsoleTitleA("Console");
		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);
		// printf("RUN\n");
		Interfaces.GetInterfaces();
		netVars = new CNetVars();

		errlog.open(Utils::GetPath() + "segt.log", std::ofstream::out | std::ofstream::app);
		Utils::log("========= cheats start =========");

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
static CVMTHookManager gDirectXHook;
static std::map<std::string, ConVar*> gConVar;
static CBaseEntity* pCurrentAiming, *pTriggerAiming;
static DWORD gModuleClient, gModuleEngine, gModuleMaterial;

static bool bImGuiInitialized = false, bBoxEsp = true, bTriggerBot = false, bAimBot = false, bBhop = true,
bRapidFire = true, bSilentAim = false, bAutoStrafe = false;
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
	Utils::log("VEngineClient 0x%X", (DWORD)Interfaces.Engine);
	Utils::log("EngineTraceClient 0x%X", (DWORD)Interfaces.Trace);
	Utils::log("VClient 0x%X", (DWORD)Interfaces.Client);
	Utils::log("VClientEntityList 0x%X", (DWORD)Interfaces.ClientEntList);
	Utils::log("VModelInfoClient 0x%X", (DWORD)Interfaces.ModelInfo);
	Utils::log("VGUI_Panel 0x%X", (DWORD)Interfaces.Panel);
	Utils::log("VGUI_Surface 0x%X", (DWORD)Interfaces.Surface);
	Utils::log("PlayerInfoManager 0x%X", (DWORD)Interfaces.PlayerInfo);
	Utils::log("VClientPrediction 0x%X", (DWORD)Interfaces.Prediction);
	Utils::log("GameMovement 0x%X", (DWORD)Interfaces.GameMovement);
	Utils::log("VDebugOverlay 0x%X", (DWORD)Interfaces.DebugOverlay);
	Utils::log("GameEventsManager 0x%X", (DWORD)Interfaces.GameEvent);
	Utils::log("VEngineModel 0x%X", (DWORD)Interfaces.ModelRender);
	Utils::log("VEngineRenderView 0x%X", (DWORD)Interfaces.RenderView);
	Utils::log("VEngineCvar 0x%X", (DWORD)Interfaces.Engine);
	Utils::log("GlobalsVariable 0x%X", (DWORD)Interfaces.Globals);
	Utils::log("Input 0x%X", (DWORD)Interfaces.Input);

	typedef void*(__stdcall* FnGetClientMode)();
	FnGetClientMode GetClientModeNormal = nullptr;
	DWORD size, address;
	address = Utils::GetModuleBase("client.dll", &size);

	if ((GetClientModeNormal = (FnGetClientMode)Utils::FindPattern(address, size, "B8 ? ? ? ? C3")) != nullptr)
	{
		Interfaces.ClientMode = GetClientModeNormal();
		if (Interfaces.ClientMode)
		{
			Interfaces.ClientModeHook = new CVMTHookManager(Interfaces.ClientMode);
			// printo("ClientModePtr", Interfaces.ClientMode);
			Utils::log("ClientModePointer = 0x%X", (DWORD)Interfaces.ClientMode);
		}
	}
	else
	{
		Interfaces.ClientMode = *(void**)(address + IClientModePointer);
		if (Interfaces.ClientMode && (DWORD)Interfaces.ClientMode > address)
		{
			Interfaces.ClientModeHook = new CVMTHookManager((void*)(*(DWORD*)Interfaces.ClientMode));
			// printo("ClientModePtr", Interfaces.ClientMode);
			Utils::log("ClientModePointer = 0x%X", (DWORD)Interfaces.ClientMode);
		}
	}

	if (Interfaces.PanelHook && indexes::PaintTraverse > -1)
	{
		oPaintTraverse = (FnPaintTraverse)Interfaces.PanelHook->HookFunction(indexes::PaintTraverse, Hooked_PaintTraverse);
		Interfaces.PanelHook->HookTable(true);
		// printo("oPaintTraverse", oPaintTraverse);
		Utils::log("oPaintTraverse = 0x%X", (DWORD)oPaintTraverse);
	}

	if (Interfaces.ClientModeHook && indexes::SharedCreateMove > -1)
	{
		oCreateMoveShared = (FnCreateMoveShared)Interfaces.ClientModeHook->HookFunction(indexes::SharedCreateMove, Hooked_CreateMoveShared);
		Interfaces.ClientModeHook->HookTable(true);
		// printo("oCreateMoveShared", oCreateMoveShared);
		Utils::log("oCreateMoveShared = 0x%X", (DWORD)oCreateMoveShared);
	}

	if (Interfaces.ClientHook && indexes::CreateMove > -1)
	{
		oCreateMove = (FnCreateMove)Interfaces.ClientHook->HookFunction(indexes::CreateMove, Hooked_CreateMove);
		Interfaces.ClientHook->HookTable(true);
		// printo("oCreateMove", oCreateMove);
		Utils::log("oCreateMove = 0x%X", (DWORD)oCreateMove);
	}

	if (Interfaces.ClientHook && indexes::FrameStageNotify > -1)
	{
		oFrameStageNotify = (FnFrameStageNotify)Interfaces.ClientHook->HookFunction(indexes::FrameStageNotify, Hooked_FrameStageNotify);
		Interfaces.ClientHook->HookTable(true);
		// printo("oFrameStageNotify", oFrameStageNotify);
		Utils::log("oFrameStageNotify = 0x%X", (DWORD)oFrameStageNotify);
	}

	if (Interfaces.ClientHook && indexes::InKeyEvent > -1)
	{
		oInKeyEvent = (FnInKeyEvent)Interfaces.ClientHook->HookFunction(indexes::InKeyEvent, Hooked_InKeyEvent);
		Interfaces.ClientHook->HookTable(true);
		// printo("oInKeyEvent", oInKeyEvent);
		Utils::log("oInKeyEvent = 0x%X", (DWORD)oInKeyEvent);
	}

	if (Interfaces.PredictionHook && indexes::RunCommand > -1)
	{
		oRunCommand = (FnRunCommand)Interfaces.ClientHook->HookFunction(indexes::RunCommand, Hooked_RunCommand);
		Interfaces.ClientHook->HookTable(true);
		// printo("oRunCommand", oRunCommand);
		Utils::log("oRunCommand = 0x%X", (DWORD)oRunCommand);
	}

	if (Interfaces.ModelRenderHook && indexes::DrawModel > -1)
	{
		oDrawModel = (FnDrawModel)Interfaces.ClientHook->HookFunction(indexes::DrawModel, Hooked_DrawModel);
		Interfaces.ClientHook->HookTable(true);
		// printo("oDrawModel", oDrawModel);
		Utils::log("oDrawModel = 0x%X", (DWORD)oDrawModel);
	}

	HMODULE tier0 = Utils::GetModuleHandleSafe("tier0.dll");
	if (tier0 != NULL)
	{
		PrintToConsole = (FnConMsg)GetProcAddress(tier0, "?ConMsg@@YAXPBDZZ");
		PrintToConsoleColor = (FnConColorMsg)GetProcAddress(tier0, "?ConColorMsg@@YAXABVColor@@PBDZZ");
		Utils::log("PrintToConsole = 0x%X", (DWORD)PrintToConsole);
		Utils::log("PrintToConsoleColor = 0x%X", (DWORD)PrintToConsoleColor);
	}

	if (Interfaces.Cvar)
	{
#ifdef USE_CVAR_CHANGE
		gConVar["sv_cheats"] = Interfaces.Cvar->FindVar("sv_cheats");
		gConVar["r_drawothermodels"] = Interfaces.Cvar->FindVar("r_drawothermodels");
		gConVar["cl_drawshadowtexture"] = Interfaces.Cvar->FindVar("cl_drawshadowtexture");
		gConVar["mat_fullbright"] = Interfaces.Cvar->FindVar("mat_fullbright");
		gConVar["sv_pure"] = Interfaces.Cvar->FindVar("sv_pure");
		gConVar["sv_consistency"] = Interfaces.Cvar->FindVar("sv_consistency");
		gConVar["mp_gamemode"] = Interfaces.Cvar->FindVar("mp_gamemode");
		gConVar["c_thirdpersonshoulder"] = Interfaces.Cvar->FindVar("c_thirdpersonshoulder");
#else
		gConVar["sv_cheats"] = nullptr;
		gConVar["r_drawothermodels"] = nullptr;
		gConVar["cl_drawshadowtexture"] = nullptr;
		gConVar["mat_fullbright"] = nullptr;
		gConVar["sv_pure"] = nullptr;
		gConVar["sv_consistency"] = nullptr;
		gConVar["mp_gamemode"] = nullptr;
		gConVar["c_thirdpersonshoulder"] = nullptr;
#endif

		Utils::log("*** ConVar ***");
		Utils::log("sv_cheats = 0x%X", (DWORD)gConVar["sv_cheats"]);
		Utils::log("r_drawothermodels = 0x%X", (DWORD)gConVar["r_drawothermodels"]);
		Utils::log("cl_drawshadowtexture = 0x%X", (DWORD)gConVar["cl_drawshadowtexture"]);
		Utils::log("mat_fullbright = 0x%X", (DWORD)gConVar["mat_fullbright"]);
		Utils::log("sv_pure = 0x%X", (DWORD)gConVar["sv_pure"]);
		Utils::log("sv_consistency = 0x%X", (DWORD)gConVar["sv_consistency"]);
		Utils::log("mp_gamemode = 0x%X", (DWORD)gConVar["mp_gamemode"]);
		Utils::log("c_thirdpersonshoulder = 0x%X", (DWORD)gConVar["c_thirdpersonshoulder"]);
		Utils::log("*** end ***");
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

		Utils::log("oReset = 0x%X", (DWORD)oReset);
		Utils::log("oPresent = 0x%X", (DWORD)oPresent);
		Utils::log("oEndScene = 0x%X", (DWORD)oEndScene);
		Utils::log("oDrawIndexedPrimitive = 0x%X", (DWORD)oDrawIndexedPrimitive);
		Utils::log("oCreateQuery = 0x%X", (DWORD)oCreateQuery);
	});

	gModuleClient = Utils::GetModuleBase("client.dll");
	gModuleEngine = Utils::GetModuleBase("engine.dll");
	gModuleMaterial = Utils::GetModuleBase("materialsystem.dll");

	Utils::log("client.dll = 0x%X", gModuleClient);
	Utils::log("engine.dll = 0x%X", gModuleEngine);
	Utils::log("materialsystem.dll = 0x%X", gModuleMaterial);
	Utils::log("localPlayer = 0x%X", (DWORD)GetLocalClient());

	// CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)autoShot, NULL, NULL, NULL);
	// CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)pure, (void*)engine, NULL, NULL);
	// CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)bunnyHop, (void*)client, NULL, NULL);
	// CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)autoPistol, NULL, NULL, NULL);
	// CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)autoAim, NULL, NULL, NULL);

	bindAlias(45);
	FreeConsole();
	Interfaces.Engine->ClientCmd("clear");

	/*
	for (;;)
	{
		if (Interfaces.Engine->IsConnected())
		{
			if (GetAsyncKeyState(VK_INSERT) & 0x01)
			{
#ifdef USE_CVAR_CHANGE
				if (gConVar["r_drawothermodels"] != nullptr)
				{
					if (gConVar["r_drawothermodels"]->IsFlagSet(FCVAR_CHEAT | FCVAR_SPONLY | FCVAR_REPLICATED | FCVAR_NOT_CONNECTED | FCVAR_SERVER_CAN_EXECUTE))
						gConVar["r_drawothermodels"]->RemoveFlags(FCVAR_CHEAT | FCVAR_SPONLY | FCVAR_REPLICATED | FCVAR_NOT_CONNECTED | FCVAR_SERVER_CAN_EXECUTE);

					if (!gConVar["r_drawothermodels"]->IsFlagSet(FCVAR_SERVER_CANNOT_QUERY))
						gConVar["r_drawothermodels"]->AddFlags(FCVAR_SERVER_CANNOT_QUERY);
				}

				if (gConVar["cl_drawshadowtexture"] != nullptr)
				{
					if (gConVar["cl_drawshadowtexture"]->IsFlagSet(FCVAR_CHEAT | FCVAR_SPONLY | FCVAR_REPLICATED | FCVAR_NOT_CONNECTED | FCVAR_SERVER_CAN_EXECUTE))
						gConVar["cl_drawshadowtexture"]->RemoveFlags(FCVAR_CHEAT | FCVAR_SPONLY | FCVAR_REPLICATED | FCVAR_NOT_CONNECTED | FCVAR_SERVER_CAN_EXECUTE);

					if (!gConVar["cl_drawshadowtexture"]->IsFlagSet(FCVAR_SERVER_CANNOT_QUERY))
						gConVar["cl_drawshadowtexture"]->AddFlags(FCVAR_SERVER_CANNOT_QUERY);
				}
#endif

#ifdef USE_CVAR_CHANGE
				if (gConVar["r_drawothermodels"] != nullptr && gConVar["cl_drawshadowtexture"] != nullptr)
				{
					if (gConVar["r_drawothermodels"]->GetInt() == 2)
					{
						gConVar["r_drawothermodels"]->SetValue(1);
						gConVar["cl_drawshadowtexture"]->SetValue(0);
					}
					else
					{
						gConVar["r_drawothermodels"]->SetValue(2);
						gConVar["cl_drawshadowtexture"]->SetValue(1);
					}

					Interfaces.Engine->ClientCmd("echo \"[ConVar] r_drawothermodels set %d\"",
						gConVar["r_drawothermodels"]->GetInt());
					Interfaces.Engine->ClientCmd("echo \"[ConVar] cl_drawshadowtexture set %d\"",
						gConVar["cl_drawshadowtexture"]->GetInt());
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

					Interfaces.Engine->ClientCmd("echo \"r_drawothermodels set %d\"",
						Utils::readMemory<int>(client + r_drawothermodels));
					Interfaces.Engine->ClientCmd("echo \"cl_drawshadowtexture set %d\"",
						Utils::readMemory<int>(client + cl_drawshadowtexture));
#ifdef USE_CVAR_CHANGE
				}
#endif
				bBoxEsp = !bBoxEsp;
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}

			if (GetAsyncKeyState(VK_HOME) & 0x01)
			{
#ifdef USE_CVAR_CHANGE
				if (gConVar["mat_fullbright"] != nullptr)
				{
					if (gConVar["mat_fullbright"]->IsFlagSet(FCVAR_CHEAT | FCVAR_SPONLY | FCVAR_REPLICATED | FCVAR_NOT_CONNECTED | FCVAR_SERVER_CAN_EXECUTE))
						gConVar["mat_fullbright"]->RemoveFlags(FCVAR_CHEAT | FCVAR_SPONLY | FCVAR_REPLICATED | FCVAR_NOT_CONNECTED | FCVAR_SERVER_CAN_EXECUTE);

					if (!gConVar["mat_fullbright"]->IsFlagSet(FCVAR_SERVER_CANNOT_QUERY))
						gConVar["mat_fullbright"]->AddFlags(FCVAR_SERVER_CANNOT_QUERY);
				}
#endif

#ifdef USE_CVAR_CHANGE
				if (gConVar["mat_fullbright"] != nullptr)
				{
					if (gConVar["mat_fullbright"]->GetInt() == 1)
						gConVar["mat_fullbright"]->SetValue(0);
					else
						gConVar["mat_fullbright"]->SetValue(1);

					Interfaces.Engine->ClientCmd("echo \"[ConVar] mat_fullbright set %d\"",
						gConVar["mat_fullbright"]->GetInt());
				}
				else
				{
#endif
					if (Utils::readMemory<int>(material + mat_fullbright) == 1)
						Utils::writeMemory(0, material + mat_fullbright);
					else
						Utils::writeMemory(1, material + mat_fullbright);

					Interfaces.Engine->ClientCmd("echo \"mat_fullbright set %d\"",
						Utils::readMemory<int>(material + mat_fullbright));
#ifdef USE_CVAR_CHANGE
				}
#endif

				std::this_thread::sleep_for(std::chrono::seconds(1));
			}

			if (GetAsyncKeyState(VK_PRIOR) & 0x01)
			{
#ifdef USE_CVAR_CHANGE
				if (gConVar["mp_gamemode"] != nullptr)
				{
					if (gConVar["mp_gamemode"]->IsFlagSet(FCVAR_CHEAT | FCVAR_SPONLY | FCVAR_REPLICATED | FCVAR_NOT_CONNECTED | FCVAR_SERVER_CAN_EXECUTE))
						gConVar["mp_gamemode"]->RemoveFlags(FCVAR_CHEAT | FCVAR_SPONLY | FCVAR_REPLICATED | FCVAR_NOT_CONNECTED | FCVAR_SERVER_CAN_EXECUTE);

					if (!gConVar["mp_gamemode"]->IsFlagSet(FCVAR_SERVER_CANNOT_QUERY))
						gConVar["mp_gamemode"]->AddFlags(FCVAR_SERVER_CANNOT_QUERY);
				}
#endif

#ifdef USE_CVAR_CHANGE
				if (gConVar["mp_gamemode"] != nullptr)
				{
					const char* mode = gConVar["mp_gamemode"]->GetString();
					if (_strcmpi(mode, "versus") == 0 || _strcmpi(mode, "realismversus") == 0)
					{
						gConVar["mp_gamemode"]->SetValue("coop");
						strcpy_s(gConVar["mp_gamemode"]->m_pszString, gConVar["mp_gamemode"]->m_StringLength, "coop");
					}
					else
					{
						gConVar["mp_gamemode"]->SetValue("versus");
						strcpy_s(gConVar["mp_gamemode"]->m_pszString, gConVar["mp_gamemode"]->m_StringLength, "versus");
					}

					Interfaces.Engine->ClientCmd("echo \"[ConVar] mp_gamemode set %s\"",
						gConVar["mp_gamemode"]->GetString());
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
							if (_strcmpi(mode, "versus") == 0 || _strcmpi(mode, "realismversus") == 0)
								strcpy_s(mode, 16, "coop");
							else
								strcpy_s(mode, 16, "versus");
							VirtualProtect(mode, sizeof(char) * 16, oldProtect, &oldProtect);
						}
						else
							printf("VirtualProtect 0x%X Fail!\n", (DWORD)mode);

						Interfaces.Engine->ClientCmd("echo \"mp_gamemode set %s\"", mode);
					}
#ifdef USE_CVAR_CHANGE
				}
#endif

				std::this_thread::sleep_for(std::chrono::seconds(1));
			}

			if (GetAsyncKeyState(VK_APPS) & 0x01)
			{
#ifdef USE_CVAR_CHANGE
				if (gConVar["sv_cheats"] != nullptr)
				{
					// 瑙ｉ櫎淇敼闄愬埗
					if (gConVar["sv_cheats"]->IsFlagSet(FCVAR_CHEAT | FCVAR_SPONLY | FCVAR_REPLICATED | FCVAR_NOT_CONNECTED | FCVAR_SERVER_CAN_EXECUTE))
						gConVar["sv_cheats"]->RemoveFlags(FCVAR_CHEAT | FCVAR_SPONLY | FCVAR_REPLICATED | FCVAR_NOT_CONNECTED | FCVAR_SERVER_CAN_EXECUTE);

					// 闃叉琚彂鐜
					if (!gConVar["sv_cheats"]->IsFlagSet(FCVAR_SERVER_CANNOT_QUERY))
						gConVar["sv_cheats"]->AddFlags(FCVAR_SERVER_CANNOT_QUERY);
				}
#endif

#ifdef USE_CVAR_CHANGE
				if (gConVar["sv_cheats"] != nullptr)
				{
					gConVar["sv_cheats"]->SetValue(1);
					gConVar["sv_cheats"]->m_fValue = 1.0f;
					gConVar["sv_cheats"]->m_nValue = 1;
					Interfaces.Engine->ClientCmd("echo \"[ConVar] sv_cheats set %d\"",
						gConVar["sv_cheats"]->GetInt());
				}
#endif

				if (Utils::readMemory<int>(engine + sv_cheats) != 1)
				{
					Utils::writeMemory(1, engine + sv_cheats);
					Interfaces.Engine->ClientCmd("echo \"sv_cheats set %d\"",
						Utils::readMemory<int>(engine + sv_cheats));
				}

				std::this_thread::sleep_for(std::chrono::seconds(1));
			}

			if (GetAsyncKeyState(VK_NEXT) & 0x01)
				thirdPerson();

			if (GetAsyncKeyState(VK_CAPITAL) & 0x01)
				showSpectator();

			if (GetAsyncKeyState(VK_F8) & 0x01)
			{
				bAutoStrafe = !bAutoStrafe;
				Interfaces.Engine->ClientCmd("echo \"[segt] auto strafe set %s\"",
					(bAutoStrafe ? "enable" : "disabled"));

				std::this_thread::sleep_for(std::chrono::seconds(1));
			}

			if (GetAsyncKeyState(VK_F9) & 0x01)
			{
				bTriggerBot = !bTriggerBot;
				Interfaces.Engine->ClientCmd("echo \"[segt] trigger bot set %s\"",
					(bTriggerBot ? "enable" : "disabled"));

				std::this_thread::sleep_for(std::chrono::seconds(1));
			}

			if (GetAsyncKeyState(VK_F10) & 0x01)
			{
				bAimBot = !bAimBot;
				Interfaces.Engine->ClientCmd("echo \"[segt] aim bot set %s\"",
					(bAimBot ? "enable" : "disabled"));

				std::this_thread::sleep_for(std::chrono::seconds(1));
			}

			if (GetAsyncKeyState(VK_F11) & 0x01)
			{
				bBhop = !bBhop;
				Interfaces.Engine->ClientCmd("echo \"[segt] auto bunnyHop set %s\"",
					(bBhop ? "enable" : "disabled"));

				std::this_thread::sleep_for(std::chrono::seconds(1));
			}

			if (GetAsyncKeyState(VK_F12) & 0x01)
			{
				bRapidFire = !bRapidFire;
				Interfaces.Engine->ClientCmd("echo \"[segt] auto pistol fire %s\"",
					(bRapidFire ? "enable" : "disabled"));

				std::this_thread::sleep_for(std::chrono::seconds(1));
			}

			if (!connected)
			{
				Interfaces.Engine->ClientCmd("echo \"********* connected *********\"");

				Utils::log("*** connected ***");
			}

			connected = true;
		}
		else if (connected && !Interfaces.Engine->IsInGame())
		{
			static bool disconnected = false;
			disconnected = true;

#ifdef USE_CVAR_CHANGE
			if (gConVar["r_drawothermodels"] != nullptr)
			{
				if (gConVar["r_drawothermodels"]->GetInt() != 1)
				{
					gConVar["r_drawothermodels"]->SetValue(1);
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
			if (gConVar["cl_drawshadowtexture"] != nullptr)
			{
				if (gConVar["cl_drawshadowtexture"]->GetInt() != 0)
				{
					gConVar["cl_drawshadowtexture"]->SetValue(0);
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
			if (gConVar["mat_fullbright"] != nullptr)
			{
				if (gConVar["mat_fullbright"]->GetInt() != 0)
				{
					gConVar["mat_fullbright"]->SetValue(0);
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
				Interfaces.Engine->ClientCmd("echo \"********* disconnected *********\"");

				Utils::log("*** disconnected ***");

				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
		}

		if (GetAsyncKeyState(VK_ADD) & 0x01)
		{
			Interfaces.Engine->ClientCmd("alias fastmelee_loop \"+attack; slot1; wait 1; -attack; slot2; wait %d; fastmelee_launcher\"", ++fmWait);
			Interfaces.Engine->ClientCmd("echo \"fastmelee wait set %d\"", fmWait);
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}

		if (GetAsyncKeyState(VK_SUBTRACT) & 0x01)
		{
			Interfaces.Engine->ClientCmd("alias fastmelee_loop \"+attack; slot1; wait 1; -attack; slot2; wait %d; fastmelee_launcher\"", --fmWait);
			Interfaces.Engine->ClientCmd("echo \"fastmelee wait set %d\"", fmWait);
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}

		if (Interfaces.Engine->IsConnected())
		{
#ifdef USE_CVAR_CHANGE
			if (gConVar["sv_pure"] != nullptr)
			{
				if (gConVar["sv_pure"]->IsFlagSet(FCVAR_CHEAT | FCVAR_SPONLY | FCVAR_REPLICATED | FCVAR_NOT_CONNECTED | FCVAR_SERVER_CAN_EXECUTE))
					gConVar["sv_pure"]->RemoveFlags(FCVAR_CHEAT | FCVAR_SPONLY | FCVAR_REPLICATED | FCVAR_NOT_CONNECTED | FCVAR_SERVER_CAN_EXECUTE);

				if (!gConVar["sv_pure"]->IsFlagSet(FCVAR_SERVER_CANNOT_QUERY))
					gConVar["sv_pure"]->AddFlags(FCVAR_SERVER_CANNOT_QUERY);
			}
#endif

#ifdef USE_CVAR_CHANGE
			if (gConVar["sv_consistency"] != nullptr)
			{
				if (gConVar["sv_consistency"]->IsFlagSet(FCVAR_CHEAT | FCVAR_SPONLY | FCVAR_REPLICATED | FCVAR_NOT_CONNECTED | FCVAR_SERVER_CAN_EXECUTE))
					gConVar["sv_consistency"]->RemoveFlags(FCVAR_CHEAT | FCVAR_SPONLY | FCVAR_REPLICATED | FCVAR_NOT_CONNECTED | FCVAR_SERVER_CAN_EXECUTE);

				if (!gConVar["sv_consistency"]->IsFlagSet(FCVAR_SERVER_CANNOT_QUERY))
					gConVar["sv_consistency"]->AddFlags(FCVAR_SERVER_CANNOT_QUERY);
			}
#endif

#ifdef USE_CVAR_CHANGE
			if (gConVar["c_thirdpersonshoulder"] != nullptr)
			{
				if (gConVar["c_thirdpersonshoulder"]->IsFlagSet(FCVAR_CHEAT | FCVAR_SPONLY | FCVAR_REPLICATED | FCVAR_NOT_CONNECTED | FCVAR_SERVER_CAN_EXECUTE))
					gConVar["c_thirdpersonshoulder"]->RemoveFlags(FCVAR_CHEAT | FCVAR_SPONLY | FCVAR_REPLICATED | FCVAR_NOT_CONNECTED | FCVAR_SERVER_CAN_EXECUTE);

				// 闃叉琚煇浜涙彃浠舵煡璇㈠埌
				if (!gConVar["c_thirdpersonshoulder"]->IsFlagSet(FCVAR_SERVER_CANNOT_QUERY))
					gConVar["c_thirdpersonshoulder"]->AddFlags(FCVAR_SERVER_CANNOT_QUERY);
			}
#endif

#ifdef USE_CVAR_CHANGE
			if (gConVar["sv_pure"] != nullptr && gConVar["sv_pure"]->GetInt() != 0)
				gConVar["sv_pure"]->SetValue(0);
			if (gConVar["sv_consistency"] != nullptr && gConVar["sv_consistency"]->GetInt() != 0)
				gConVar["sv_consistency"]->SetValue(0);
#endif

			if (Utils::readMemory<int>(engine + sv_pure) != 0 ||
				Utils::readMemory<int>(engine + sv_consistency) != 0)
			{
				Utils::writeMemory(0, engine + sv_pure);
				Utils::writeMemory(0, engine + sv_consistency);

				Interfaces.Engine->ClientCmd("echo \"sv_pure and sv_consistency set %d\"",
					Utils::readMemory<int>(engine + sv_pure));

				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
		}

		if (GetAsyncKeyState(VK_END) & 0x01)
		{
			errlog.close();
			ExitProcess(0);
		}

		if (GetAsyncKeyState(VK_DELETE) & 0x01)
			Interfaces.Engine->ClientCmd("disconnect");

		std::this_thread::sleep_for(std::chrono::microseconds(1));
	}
	*/
}

bool IsAliveTarget(CBaseEntity* entity)
{
	int id = 0, solid = 0, sequence = 0;
	ClientClass* cc = nullptr;

	try
	{
		if (entity == nullptr || entity->IsDormant())
			return false;

		cc = entity->GetClientClass();
		if (cc == nullptr)
			return false;

		id = cc->m_ClassID;
		solid = entity->GetNetProp<int>("m_usSolidFlags", "DT_BaseCombatCharacter");
		sequence = entity->GetNetProp<int>("m_nSequence", "DT_BaseCombatCharacter");
	}
	catch (...)
	{
		return false;
	}

	if (id == ET_BOOMER || id == ET_HUNTER || id == ET_SMOKER || id == ET_SPITTER ||
		id == ET_JOCKEY || id == ET_CHARGER || id == ET_TANK)
	{
		if (!entity->IsAlive() || entity->GetNetProp<byte>("m_isGhost", "DT_TerrorPlayer") != 0)
		{
#ifdef _DEBUG
			Interfaces.Engine->ClientCmd("echo \"Special 0x%X healh = %d, ghost = %d\"", (DWORD)entity,
				entity->GetNetProp<int>("m_iHealth", "DT_TerrorPlayer"), entity->GetNetProp<int>("m_isGhost", "DT_TerrorPlayer"));
#endif
			return false;
		}
	}
	else if (id == ET_INFECTED || id == ET_WITCH)
	{
		if ((solid & SF_NOT_SOLID))
		{
#ifdef _DEBUG
			Interfaces.Engine->ClientCmd("echo \"Common 0x%X is dead\"", (DWORD)entity);
#endif
			return false;
		}
	}
	else if (id == ET_CTERRORPLAYER || id == ET_SURVIVORBOT)
	{
		if (!entity->IsAlive())
		{
#ifdef _DEBUG
			Interfaces.Engine->ClientCmd("echo \"Survivor 0x%X is dead %d\"", (DWORD)entity,
				entity->GetNetProp<int>("m_iHealth", "DT_TerrorPlayer"));
#endif
			return false;
		}
	}
	else
	{
#ifdef _DEBUG
		// Utils::log("Invalid ClassId = %d | sequence = %d | solid = %d", id, sequence, solid);
		Interfaces.Engine->ClientCmd("echo \"Invalid Entity 0x%X ClassId %d\"", (DWORD)entity, id);
#endif
		return false;
	}

	return true;
}

std::string GetZombieClassName(CBaseEntity* player)
{
	if (player == nullptr || player->IsDormant())
		return "";

	if (player->GetClientClass()->m_ClassID == ET_INFECTED)
	{
		int zombie = player->GetNetProp<int>("m_Gender", "DT_Infected");
		switch (zombie)
		{
			/*
			case 1:
			return "male";
			case 2:
			return "female";
			*/
		case 11:
			// 防火 CEDA 人员
			return "ceda";
		case 12:
			// 泥人
			return "mud";
		case 13:
			// 修路工人
			return "roadcrew";
		case 14:
			// 被感染的幸存者
			return "fallen";
		case 15:
			// 防暴警察
			return "riot";
		case 16:
			// 小丑
			return "clown";
		case 17:
			// 赛车手吉米
			return "jimmy";
		}

		// 常见感染者
		return "infected";
	}
	if (player->GetClientClass()->m_ClassID == ET_WITCH)
	{
		// 新娘 Witch
		if (player->GetNetProp<int>("m_Gender", "DT_Infected") == 19)
			return "bride";

		// 普通 Witch
		return "witch";
	}

	int zombie = player->GetNetProp<int>("m_zombieClass", "DT_TerrorPlayer");
	int character = player->GetNetProp<int>("m_survivorCharacter", "DT_TerrorPlayer");

	switch (zombie)
	{
	case ZC_SMOKER:
		// 舌头
		return "smoker";
	case ZC_BOOMER:
		// 胖子
		return "boomer";
	case ZC_HUNTER:
		// 猎人
		return "hunter";
	case ZC_SPITTER:
		// 口水
		return "spitter";
	case ZC_JOCKEY:
		// 猴
		return "jockey";
	case ZC_CHARGER:
		// 牛
		return "charger";
	case ZC_TANK:
		// 克
		return "tank";
	case ZC_SURVIVORBOT:
		switch (character)
		{
		case 0:
			// 西装
			return "nick";
		case 1:
			// 黑妹
			return "rochelle";
		case 2:
			// 黑胖
			return "coach";
		case 3:
			// 帽子
			return "ellis";
		case 4:
			// 老头
			return "bill";
		case 5:
			// 女人
			return "zoey";
		case 6:
			// 马甲
			return "francis";
		case 7:
			// 光头
			return "louis";
		}
	}

	// 不知道
	return "unknown";
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

Vector GetHeadHitboxPosition(CBaseEntity* entity)
{
	Vector position;

	if (!IsAliveTarget(entity))
		return position;

	int classId = entity->GetClientClass()->m_ClassID;
	switch (classId)
	{
	case ET_TANK:
	case ET_WITCH:
	case ET_SMOKER:
	case ET_BOOMER:
	case ET_HUNTER:
	case ET_CTERRORPLAYER:
	case ET_SURVIVORBOT:
		position = entity->GetHitboxPosition(HITBOX_PLAYER);
		break;
	case ET_JOCKEY:
	case ET_SPITTER:
		position = entity->GetHitboxPosition(HITBOX_JOCKEY);
		break;
	case ET_CHARGER:
		position = entity->GetHitboxPosition(HITBOX_CHARGER);
		break;
	case ET_INFECTED:
		position = entity->GetHitboxPosition(HITBOX_COMMON);
		break;
	}

	if (!position.IsValid())
		position = GetHeadPosition(entity);

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

#ifdef _DEBUG
	Utils::log("TraceRay: skip = 0x%X | start = (%.2f %.2f %.2f) | end = (%.2f %.2f %.2f)",
		(DWORD)client, src.x, src.y, src.z, dst.x, dst.y, dst.z);
#endif

	Interfaces.Trace->TraceRay(ray, MASK_SHOT, &filter, &trace);

#ifdef _DEBUG
	Utils::log("TraceRay: entity = 0x%X | hitbox = %d | bone = %d | hitGroup = %d | fraction = %.2f | classId = %d",
		trace.m_pEnt, trace.hitbox, trace.physicsBone, trace.hitGroup, trace.fraction,
		(trace.m_pEnt != nullptr ? trace.m_pEnt->GetClientClass()->m_ClassID : -1));
#endif

	// 妫�鏌ョ洰鏍囨槸鍚︿负涓�涓湁鏁堢殑瀹炰綋
	if (trace.m_pEnt == nullptr || trace.m_pEnt->IsDormant() ||
		trace.m_pEnt->GetClientClass()->m_ClassID == ET_WORLD)
	{
#ifdef _DEBUG
		Interfaces.Engine->ClientCmd("echo \"invalid entity 0x%X\"", (DWORD)trace.m_pEnt);
#endif
		return nullptr;
	}

	// 妫�鏌ョ洰鏍囨槸鍚︿负涓�涓彲瑙佺殑鐗╀綋锛屾垨鑰呰鐗╀綋鏄惁鍙互琚嚮涓紝浠ュ強鏄惁涓烘寚瀹氫綅
	if (trace.hitbox == 0 || (hitbox > 0 && trace.hitbox != hitbox))
	{
#ifdef _DEBUG
		Interfaces.Engine->ClientCmd("echo \"invalid hitbox 0x%X | hitbox = %d | bone = %d | group = %d\"",
			(DWORD)trace.m_pEnt, trace.hitbox, trace.physicsBone, trace.hitGroup);
#endif

		return nullptr;
	}

	if (bAimBot && hitbox <= 0)
	{
		int classId = trace.m_pEnt->GetClientClass()->m_ClassID;
		switch (classId)
		{
		case ET_TANK:
		case ET_WITCH:
		case ET_SMOKER:
		case ET_BOOMER:
		case ET_HUNTER:
		case ET_CTERRORPLAYER:
		case ET_SURVIVORBOT:
			if (trace.hitbox == HITBOX_PLAYER)
				goto result_success;
			break;
		case ET_JOCKEY:
		case ET_SPITTER:
			if (trace.hitbox == HITBOX_JOCKEY)
				goto result_success;
			break;
		case ET_CHARGER:
			if (trace.hitbox == HITBOX_CHARGER)
				goto result_success;
			break;
		case ET_INFECTED:
			if (trace.hitbox >= HITBOX_COMMON_1 && trace.hitbox <= HITBOX_COMMON_4)
				goto result_success;
			break;
		}

		return nullptr;
	}

result_success:
	return trace.m_pEnt;
}

bool IsTargetVisible(CBaseEntity* entity, const Vector& end = Vector())
{
	CBaseEntity* client = GetLocalClient();
	if (client == nullptr || !Interfaces.Engine->IsInGame())
		return false;

	trace_t trace;
	Ray_t ray;

	CTraceFilter filter;
	filter.pSkip1 = client;

	if (end.IsValid())
		ray.Init(client->GetEyePosition(), end);
	else if (entity->GetClientClass()->m_ClassID == ET_INFECTED)
		ray.Init(client->GetEyePosition(), entity->GetHitboxPosition(HITBOX_COMMON));
	else
		ray.Init(client->GetEyePosition(), entity->GetHitboxPosition(HITBOX_PLAYER));

	Interfaces.Trace->TraceRay(ray, MASK_SHOT, &filter, &trace);

	// 妫�鏌ョ洰鏍囨槸鍚︿负涓�涓湁鏁堢殑瀹炰綋
	if (trace.m_pEnt == nullptr || trace.m_pEnt->IsDormant() ||
		trace.m_pEnt->GetClientClass()->m_ClassID == ET_WORLD)
		return false;

	// 妫�鏌ョ洰鏍囨槸鍚︿负涓�涓彲瑙佺殑鐗╀綋锛屾垨鑰呰鐗╀綋鏄惁鍙互琚嚮涓紝浠ュ強鏄惁涓烘寚瀹氫綅缃
	if (trace.hitbox == 0)
		return false;

	return true;
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

	Utils::log("========= Hook D3D Device =========");
	Utils::log("pD3DDevice = 0x%X", dh::gDeviceInternal);
	Utils::log("oReset = 0x%X", oReset);
	Utils::log("oPresent = 0x%X", oPresent);
	Utils::log("oEndScene = 0x%X", oEndScene);
	Utils::log("oDrawIndexedPrimitive = 0x%X", oDrawIndexedPrimitive);
	Utils::log("oCreateQuery = 0x%X", oCreateQuery);
	Utils::log("========= Hook Device End =========");
	Interfaces.Engine->ClientCmd("clear");
}

void pure(void* engine)
{
	for (;;)
	{
		if (gConVar["sv_pure"] != nullptr)
		{
			if (gConVar["sv_pure"]->IsFlagSet(FCVAR_CHEAT | FCVAR_SPONLY | FCVAR_REPLICATED | FCVAR_NOT_CONNECTED | FCVAR_SERVER_CAN_EXECUTE))
				gConVar["sv_pure"]->RemoveFlags(FCVAR_CHEAT | FCVAR_SPONLY | FCVAR_REPLICATED | FCVAR_NOT_CONNECTED | FCVAR_SERVER_CAN_EXECUTE);
		}

		if (gConVar["sv_consistency"] != nullptr)
		{
			if (gConVar["sv_consistency"]->IsFlagSet(FCVAR_CHEAT | FCVAR_SPONLY | FCVAR_REPLICATED | FCVAR_NOT_CONNECTED | FCVAR_SERVER_CAN_EXECUTE))
				gConVar["sv_consistency"]->RemoveFlags(FCVAR_CHEAT | FCVAR_SPONLY | FCVAR_REPLICATED | FCVAR_NOT_CONNECTED | FCVAR_SERVER_CAN_EXECUTE);
		}

		if (gConVar["sv_pure"] != nullptr && gConVar["sv_pure"]->GetInt() != 0)
			gConVar["sv_pure"]->SetValue(0);
		if (gConVar["sv_consistency"] != nullptr && gConVar["sv_consistency"]->GetInt() != 0)
			gConVar["sv_consistency"]->SetValue(0);

		if (Utils::readMemory<int>((DWORD)engine + sv_pure) != 0 ||
			Utils::readMemory<int>((DWORD)engine + sv_consistency) != 0)
		{
			Utils::writeMemory(0, (DWORD)engine + sv_pure);
			Utils::writeMemory(0, (DWORD)engine + sv_consistency);

			Interfaces.Engine->ClientCmd("echo \"sv_pure and sv_consistency set %d\"",
				Utils::readMemory<int>((DWORD)engine + sv_pure));
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

void __stdcall Hooked_CreateMove(int sequence_number, float input_sample_frametime, bool active)
{
	static bool showHint = true;
	if (showHint)
	{
		// showHint = false;
		std::cout << "Hooked_CreateMove trigged." << std::endl;
		Utils::log("Hooked_CreateMove success");
	}

	oCreateMove(sequence_number, input_sample_frametime, active);

	DWORD dwEBP = NULL;
	__asm mov dwEBP, ebp
	bool* bSendPacket = (bool*)(*(char**)dwEBP - 0x21);

	CVerifiedUserCmd *pVerifiedCmd = &(*(CVerifiedUserCmd**)((DWORD)Interfaces.Input + 0xE0))[sequence_number % 150];
	CUserCmd *pCmd = &(*(CUserCmd**)((DWORD_PTR)Interfaces.Input + 0xDC))[sequence_number % 150];
	if (showHint)
	{
		showHint = false;
		Utils::log("pVerifiedCmd = 0x%X", (DWORD)pVerifiedCmd);
		Utils::log("pCmd = 0x%X", (DWORD)pCmd);
	}

	CBaseEntity* client = GetLocalClient();
	if (client == nullptr || pCmd == nullptr || pVerifiedCmd == nullptr || !client->IsAlive() ||
		Interfaces.Engine->IsConsoleVisible())
		return;

	float serverTime = GetServerTime();
	CBaseEntity* weapon = (CBaseEntity*)client->GetActiveWeapon();
	if (weapon != nullptr)
		weapon = Interfaces.ClientEntList->GetClientEntityFromHandle(weapon);
	else
		weapon = nullptr;

	QAngle oldViewAngles = pCmd->viewangles;
	float oldSidemove = pCmd->sidemove;
	float oldForwardmove = pCmd->fowardmove;

	// 自动连跳
	if (bBhop && (GetAsyncKeyState(VK_SPACE) & 0x8000))
	{
		static bool lastJump = false;
		static bool shouldFake = false;

		if (!lastJump && shouldFake)
		{
			shouldFake = false;
			pCmd->buttons |= IN_JUMP;
		}
		else if (pCmd->buttons & IN_JUMP)
		{
			if (client->GetFlags() & FL_ONGROUND)
			{
				lastJump = true;
				shouldFake = true;
			}
			else
			{
				pCmd->buttons &= ~IN_JUMP;
				lastJump = false;
			}
		}
		else
		{
			lastJump = false;
			shouldFake = false;
		}

		// 杩炶烦鑷姩鍚戝墠绉诲姩锛堜笉闇�瑕佹寜鍏朵粬閿級
		if (bAutoStrafe && !(client->GetFlags() & FL_ONGROUND))
		{
			if (pCmd->mousedx < 0)
				pCmd->sidemove = -400.f;

			if (pCmd->mousedx > 0)
				pCmd->sidemove = 400.f;
		}
	}

	// 自动瞄准
	if (bAimBot && GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		Vector myOrigin = client->GetEyePosition(), myAngles = pCmd->viewangles;

		// 瀵绘壘鐩爣
		/*
		if (!IsAliveTarget(pCurrentAiming))
		{
		int team = client->GetTeam();
		float distance = 32767.0f, dist, fov;
		// bool visible = false;
		Vector headPos;

		pCurrentAiming = nullptr;
		int maxEntity = Interfaces.Engine->GetMaxClients();
		for (int i = 1; i < maxEntity; ++i)
		{
		CBaseEntity* target = Interfaces.ClientEntList->GetClientEntity(i);

		// 妫�鏌ユ槸鍚︿负涓�涓湁鏁堢殑骞朵笖娲荤潃鐨勬晫浜
		if (!IsAliveTarget(target) || target->GetTeam() == team)
		{
		// Interfaces.Engine->ClientCmd("echo \"Invalid Entity 0x%X\"", (DWORD)target);
		continue;
		}

		// 浼樺厛閫夋嫨鐗规劅
		if (i > 64 && pCurrentAiming != nullptr)
		break;

		int classId = target->GetClientClass()->m_ClassID;

		if (classId == ET_INFECTED)
		{
		// 鐗规劅鏄笉闇�瑕佺瀯鍑嗘櫘鎰熺殑
		if (team == 3)
		continue;

		// 鏅劅鐨勫ご閮ㄧ殑 hitbox 鎴栬 16 涔熷彲浠
		headPos = target->GetHitboxPosition(HITBOX_COMMON);
		}
		else if (classId == ET_TANKROCK)
		{
		// Tank 鎶曟幏鐨勭煶澶达紝鍙互琚墦鐖
		headPos = target->GetNetProp<Vector>("m_vecOrigin", "DT_BaseEntity");
		}
		else
		{
		// 鐢熻繕鑰?鐗规劅
		headPos = target->GetHitboxPosition(HITBOX_PLAYER);
		}

		dist = headPos.DistTo(myOrigin);
		fov = GetAnglesFieldOfView(myAngles, CalculateAim(myOrigin, headPos));

		// 閫夋嫨鏈�杩戠殑锛岄潬杩戝噯鏄燂紝骞朵笖鍙互鐪嬭鐨勭洰鏍
		if (IsTargetVisible(target, headPos) && dist < distance && fov <= 30.f)
		{
		pCurrentAiming = target;
		distance = dist;
		}
		}

		if (pCurrentAiming != nullptr)
		Interfaces.Engine->ClientCmd("echo \"target selected 0x%X | classId = %d | health = %d\"",
		pCurrentAiming, pCurrentAiming->GetClientClass()->m_ClassID, pCurrentAiming->GetHealth());
		}
		*/

		// 鐬勫噯
		if (pCurrentAiming != nullptr)
		{
			// 鐩爣浣嶇疆
			Vector position;
			try
			{
				position = GetHeadHitboxPosition(pCurrentAiming);
			}
			catch (...)
			{
				if (pCurrentAiming->GetClientClass()->m_ClassID == ET_INFECTED)
					goto end_aimbot;

				// 鑾峰彇楠ㄩ浣嶇疆澶辫触
				position = pCurrentAiming->GetEyePosition();
				Utils::log("CBasePlayer::SetupBone error");

				// 鏍规嵁涓嶅悓鐨勬儏鍐电‘瀹氶珮搴
				int zombieClass = pCurrentAiming->GetNetProp<int>("m_zombieClass", "DT_TerrorPlayer");
				if (zombieClass == ZC_JOCKEY)
					position.z = pCurrentAiming->GetAbsOrigin().z + 30.0f;
				else if (zombieClass == ZC_HUNTER && (pCurrentAiming->GetFlags() & FL_DUCKING))
					position.z -= 12.0f;
			}

			// 閫熷害棰勬祴锛屼細瀵艰嚧灞忓箷鏅冨姩锛屾墍浠ヤ笉闇�瑕
			// position += (pCurrentAiming->GetVelocity() * Interfaces.Globals->interval_per_tick);

			// Interfaces.Engine->SetViewAngles(CalculateAim(myOrigin, position));

			if (position.IsValid())
				pCmd->viewangles = CalculateAim(myOrigin, position);
		}
	}

end_aimbot:

	// 自动开枪
	if (bTriggerBot && !(pCmd->buttons & IN_USE) && weapon != nullptr)
	{
		CBaseEntity* target = GetAimingTarget();

#ifdef _DEBUG
		if (target != nullptr)
		{
			if (!IsAliveTarget(target))
				Interfaces.Engine->ClientCmd("echo \"aiming dead 0x%X\"", (DWORD)target);
			if (target->GetTeam() == client->GetTeam())
				Interfaces.Engine->ClientCmd("echo \"aiming team 0x%X\"", (DWORD)target);
			if (target->GetClientClass()->m_ClassID == ET_INFECTED)
				Interfaces.Engine->ClientCmd("echo \"aiming infected 0x%X\"", (DWORD)target);
		}
#endif

		int weaponId = weapon->GetWeaponID();
		if (target != nullptr && IsAliveTarget(target) && target->GetTeam() != client->GetTeam() &&
			(client->GetTeam() == 2 || target->GetClientClass()->m_ClassID != ET_INFECTED) &&
			IsGunWeapon(weaponId) && target->GetClientClass()->m_ClassID != ET_WITCH)
			pCmd->buttons |= IN_ATTACK;
	}

	if (bRapidFire && weapon != nullptr && pCmd->buttons & IN_ATTACK)
	{
		int weaponId = weapon->GetWeaponID();
		float nextAttack = weapon->GetNetProp<float>("m_flNextPrimaryAttack", "DT_BaseCombatWeapon");

		static bool ignoreButton = true;
		if (IsWeaponSingle(weaponId) && !ignoreButton && nextAttack > serverTime)
		{
			pCmd->buttons &= ~IN_ATTACK;
			ignoreButton = true;
		}
		else
		{
			// 忽略一次阻止开枪
			ignoreButton = false;
		}
	}

	// 在开枪前检查，防止攻击队友
	if ((pCmd->buttons & IN_ATTACK) && weapon != nullptr)
	{
		int cid = client->GetCrosshairsId();
		CBaseEntity* aiming = (cid > 0 ? Interfaces.ClientEntList->GetClientEntity(cid) : nullptr);
		int weaponId = weapon->GetWeaponID();

		if (aiming != nullptr && !aiming->IsDormant() && aiming->IsAlive() && IsGunWeapon(weaponId) &&
			aiming->GetTeam() == client->GetTeam() && !IsNeedRescue(aiming))
		{
			// 取消开枪
			pCmd->buttons &= ~IN_ATTACK;
		}
	}

	// 没什么用
	if (bSilentAim && weapon != nullptr)
	{
		if ((pCmd->buttons & IN_ATTACK) &&
			weapon->GetNetProp<float>("m_flNextPrimaryAttack", "DT_BaseCombatWeapon") <= serverTime)
			*bSendPacket = false;
		else
		{
			*bSendPacket = true;
			pCmd->viewangles = oldViewAngles;
			pCmd->sidemove = oldSidemove;
			pCmd->fowardmove = oldForwardmove;
		}
	}

	// 获取武器Id
	/*
	if ((pCmd->buttons & IN_RELOAD) && weapon != nullptr)
	{
	static int oldId = 0;
	int weaponId = weapon->GetWeaponID();
	if (oldId != weaponId)
	{
	Interfaces.Engine->ClientCmd("echo \"current weapon id = %d\"", weaponId);
	oldId = weaponId;
	}
	}
	*/

	// 修复角度不正确
	ClampAngles(pCmd->viewangles);
	AngleNormalize(pCmd->viewangles);

	// 发送到服务器
	pVerifiedCmd->m_cmd = *pCmd;
	pVerifiedCmd->m_crc = pCmd->GetChecksum();
}

bool __stdcall Hooked_CreateMoveShared(float flInputSampleTime, CUserCmd* cmd)
{
	static bool showHint = true;
	if (showHint)
	{
		showHint = false;
		std::cout << "Hooked_CreateMoveShared trigged." << std::endl;
		Utils::log("Hooked_CreateMoveShared success");
	}

	CBaseEntity* client = GetLocalClient();

	if (!client || !cmd)
		return false;

	// 杩炶烦
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

#define FONT_SIZE 16
void __fastcall Hooked_PaintTraverse(void* pPanel, void* edx, unsigned int panel, bool forcePaint, bool allowForce)
{
	static bool showHint = true;
	if (showHint)
	{
		showHint = false;
		std::cout << "Hooked_PaintTraverse trigged." << std::endl;
		Utils::log("Hooked_PaintTraverse success");
	}

	static unsigned int MatSystemTopPanel = 0;
	static unsigned int FocusOverlayPanel = 0;

	if (MatSystemTopPanel == 0 || FocusOverlayPanel == 0)
	{
		const char* panelName = Interfaces.Panel->GetName(panel);
		if (panelName[0] == 'M' && panelName[3] == 'S' && panelName[9] == 'T')
		{
			MatSystemTopPanel = panel;
			Utils::log("panel %s found %d", panelName, MatSystemTopPanel);
		}
		else if (panelName[0] == 'F' && panelName[5] == 'O')
		{
			FocusOverlayPanel = panel;
			Utils::log("panel %s found %d", panelName, FocusOverlayPanel);
		}
	}

	static unsigned long font = 0;
	if (font == 0)
	{
		font = Interfaces.Surface->SCreateFont();
		Interfaces.Surface->SetFontGlyphSet(font, "arial", FONT_SIZE, FW_DONTCARE, 0, 0, 0x200);
	}

	if (FocusOverlayPanel > 0 && panel == FocusOverlayPanel)
	{

	}

	if (MatSystemTopPanel > 0 && panel == MatSystemTopPanel)
	{
		CBaseEntity* local = GetLocalClient();
		if (bBoxEsp && local != nullptr && Interfaces.Engine->IsInGame())
		{
			float distmin = 65535.0f;
			Vector myOrigin = local->GetEyePosition();
			int maxEntity = Interfaces.Engine->GetMaxClients();

			for (int i = 1; i < maxEntity; ++i)
			{
				CBaseEntity* entity = Interfaces.ClientEntList->GetClientEntity(i);
				if (entity == nullptr || entity->IsDormant() || (DWORD)entity == (DWORD)local ||
					!IsAliveTarget(entity))
					continue;

				Vector head, foot, headbox;
				int classId = entity->GetClientClass()->m_ClassID;
				headbox = GetHeadHitboxPosition(entity);

				if (!headbox.IsValid() || !WorldToScreen(headbox, head) ||
					!WorldToScreen(entity->GetAbsOrigin(), foot))
					continue;

				std::wstringstream ss;

				// 去除 float 的小数位，因为没必要
				ss << std::setprecision(0);

				if (classId != ET_INFECTED && classId != ET_WITCH)
				{
					if (classId == ET_SURVIVORBOT || classId == ET_CTERRORPLAYER)
					{
						if (IsIncapacitated(entity))
						{
							// 倒地时只有普通血量
							ss << L"[" << entity->GetHealth() << L" + incap] ";
						}
						else if (IsControlled(entity))
						{
							// 玩家被控了
							ss << L"[" << entity->GetHealth() +
								entity->GetNetProp<float>("m_healthBuffer", "DT_TerrorPlayer") <<
								L" + grabbed] ";
						}
						else
						{
							// 生还者显示血量，临时血量
							ss << L"[" << entity->GetHealth() << L" + " <<
								entity->GetNetProp<float>("m_healthBuffer", "DT_TerrorPlayer") << "] ";
						}
					}
					else
					{
						if (entity->GetNetProp<byte>("m_isGhost", "DT_TerrorPlayer") != 0)
						{
							// 幽灵状态的特感
							ss << L"[" << entity->GetHealth() << L" ghost] ";
						}
						else
						{
							// 非生还者只显示血量就好了
							ss << L"[" << entity->GetHealth() << L"] ";
						}
					}
				}

				// 玩家类型
				ss << Utils::c2w(GetZombieClassName(entity));

				float height = fabs(head.y - foot.y);
				float width = height * 0.65f;

				// 根据情况决定颜色
				if ((classId == ET_SURVIVORBOT || classId == ET_CTERRORPLAYER) &&
					entity->GetNetProp<byte>("m_bIsOnThirdStrike", "DT_TerrorPlayer") != 0)
				{
					// 生还者黑白时使用白色
					Interfaces.Surface->drawString(foot.x - width / 2, head.y, 255, 255, 255, font, ss.str().c_str());
				}
				else if ((classId == ET_BOOMER || classId == ET_SMOKER || classId == ET_HUNTER ||
					classId == ET_SPITTER || classId == ET_CHARGER || classId == ET_JOCKEY) &&
					entity->GetNetProp<byte>("m_isGhost", "DT_TerrorPlayer") != 0)
				{
					// 幽灵状态的特感，紫色
					Interfaces.Surface->drawString(foot.x - width / 2, head.y, 128, 0, 128, font, ss.str().c_str());
				}
				else
				{
					// 其他情况，橙色
					Interfaces.Surface->drawString(foot.x - width / 2, head.y, 255, 128, 0, font, ss.str().c_str());
				}

				ss.str(L"");

				bool visible = IsTargetVisible(entity, headbox);
				float dist = local->GetAbsOrigin().DistTo(entity->GetAbsOrigin());

				// 显示距离
				ss << dist;

				// 给生还者显示弹药
				if (classId == ET_SURVIVORBOT || classId == ET_CTERRORPLAYER)
				{
					CBaseEntity* weapon = (CBaseEntity*)entity->GetActiveWeapon();
					if (weapon != nullptr)
						weapon = Interfaces.ClientEntList->GetClientEntityFromHandle(weapon);
					if (weapon != nullptr)
					{
						int ammoType = weapon->GetNetProp<int>("m_iPrimaryAmmoType", "DT_BaseCombatWeapon");
						int clip = weapon->GetNetProp<int>("m_iClip1", "DT_BaseCombatWeapon");
						// int* ammo = entity->GetNetProp<int*>("m_iAmmo", "DT_TerrorPlayer");
						byte reloading = weapon->GetNetProp<byte>("m_bInReload", "DT_BaseCombatWeapon");

						// 显示弹药和弹夹
						if (ammoType > 0 && clip > -1)
						{
							if (reloading != 0)
							{
								// 正在换子弹
								ss << " (reloading)";
							}
							else
							{
								// 没有换子弹
								ss << " (" << clip << ")";
							}
						}
					}
				}

				// 检查是否可以看见
				if (visible)
				{
					// 看得见，显示蓝色
					Interfaces.Surface->drawString(foot.x - width / 2, head.y + FONT_SIZE, 0, 255, 255, font, ss.str().c_str());
				}
				else
				{
					// 看不见，显示黄色
					Interfaces.Surface->drawString(foot.x - width / 2, head.y + FONT_SIZE, 255, 255, 0, font, ss.str().c_str());
				}

				// 绘制一个框（虽然这个框只有上下两条线）
				Interfaces.Surface->drawBox(foot.x - width / 2, foot.y, width, -height,
					1, 255, 0, 0, 255);

				// 给 Aimbot 寻找目标
				if (bAimBot && (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000) || !IsAliveTarget(pCurrentAiming)))
				{
					pCurrentAiming = nullptr;
					if (entity->GetTeam() != local->GetTeam())
					{
						Vector myAngles;
						Interfaces.Engine->GetViewAngles(myAngles);
						if (dist < distmin && visible &&
							GetAnglesFieldOfView(myAngles, CalculateAim(myOrigin, headbox)) <= 30.0f)
						{
							pCurrentAiming = entity;
							distmin = dist;
						}
					}
				}
			}
		}
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
		std::cout << "Hooked_FrameStageNotify trigged." << std::endl;
		Utils::log("Hooked_FrameStageNotify success");
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

	static time_t nextUpdate = 0;
	time_t currentTime = time(NULL);
	if (nextUpdate <= currentTime)
	{
		nextUpdate = currentTime + 1;
		static DWORD fmWait = 45;
		static bool connected = false;

		if (Interfaces.Engine->IsConnected())
		{
			// 修改 r_drawothermodels 实现简单的 esp
			if (GetAsyncKeyState(VK_INSERT) & 0x01)
			{
#ifdef USE_CVAR_CHANGE
				CVAR_MAKE_FLAGS("r_drawothermodels");
				CVAR_MAKE_FLAGS("cl_drawshadowtexture");
#endif

#ifdef USE_CVAR_CHANGE
				if (gConVar["r_drawothermodels"] != nullptr && gConVar["cl_drawshadowtexture"] != nullptr)
				{
					CVAR_MAKE_VALUE("r_drawothermodels", 2, 1);
					CVAR_MAKE_VALUE("cl_drawshadowtexture", 1, 0);
				}
				else
				{
#endif
					if (Utils::readMemory<int>(gModuleClient + r_drawothermodels) == 2)
					{
						Utils::writeMemory(1, gModuleClient + r_drawothermodels);
						Utils::writeMemory(0, gModuleClient + cl_drawshadowtexture);
					}
					else
					{
						Utils::writeMemory(2, gModuleClient + r_drawothermodels);
						Utils::writeMemory(1, gModuleClient + cl_drawshadowtexture);
					}

					Interfaces.Engine->ClientCmd("echo \"r_drawothermodels set %d\"",
						Utils::readMemory<int>(gModuleClient + r_drawothermodels));
					Interfaces.Engine->ClientCmd("echo \"cl_drawshadowtexture set %d\"",
						Utils::readMemory<int>(gModuleClient + cl_drawshadowtexture));
#ifdef USE_CVAR_CHANGE
				}
#endif
				bBoxEsp = !bBoxEsp;
				// std::this_thread::sleep_for(std::chrono::seconds(1));
			}

			// 修改 mat_fullbright 实现全图高亮
			if (GetAsyncKeyState(VK_HOME) & 0x01)
			{
#ifdef USE_CVAR_CHANGE
				CVAR_MAKE_FLAGS("mat_fullbright");
#endif

#ifdef USE_CVAR_CHANGE
				if (gConVar["mat_fullbright"] != nullptr)
				{
					CVAR_MAKE_VALUE("mat_fullbright", 1, 0);
				}
				else
				{
#endif
					if (Utils::readMemory<int>(gModuleMaterial + mat_fullbright) == 1)
						Utils::writeMemory(0, gModuleMaterial + mat_fullbright);
					else
						Utils::writeMemory(1, gModuleMaterial + mat_fullbright);

					Interfaces.Engine->ClientCmd("echo \"mat_fullbright set %d\"",
						Utils::readMemory<int>(gModuleMaterial + mat_fullbright));
#ifdef USE_CVAR_CHANGE
				}
#endif

				// std::this_thread::sleep_for(std::chrono::seconds(1));
			}

			// 修改 mp_gamemode 在对抗模式开启第三人称
			if (GetAsyncKeyState(VK_PRIOR) & 0x01)
			{
#ifdef USE_CVAR_CHANGE
				CVAR_MAKE_FLAGS("mp_gamemode");
#endif

#ifdef USE_CVAR_CHANGE
				if (gConVar["mp_gamemode"] != nullptr)
				{
					const char* mode = gConVar["mp_gamemode"]->GetString();
					if (_strcmpi(mode, "versus") == 0 || _strcmpi(mode, "realismversus") == 0)
					{
						gConVar["mp_gamemode"]->SetValue("coop");
						strcpy_s(gConVar["mp_gamemode"]->m_pszString, gConVar["mp_gamemode"]->m_StringLength, "coop");
					}
					else
					{
						gConVar["mp_gamemode"]->SetValue("versus");
						strcpy_s(gConVar["mp_gamemode"]->m_pszString, gConVar["mp_gamemode"]->m_StringLength, "versus");
					}

					Interfaces.Engine->ClientCmd("echo \"[ConVar] mp_gamemode set %s\"",
						gConVar["mp_gamemode"]->GetString());
				}
				else
				{
#endif
					char* mode = Utils::readMemory<char*>(gModuleClient + mp_gamemode);
					if (mode != nullptr)
					{
						DWORD oldProtect = NULL;

						if (VirtualProtect(mode, sizeof(char) * 16, PAGE_EXECUTE_READWRITE, &oldProtect) == TRUE)
						{
							if (_strcmpi(mode, "versus") == 0 || _strcmpi(mode, "realismversus") == 0)
								strcpy_s(mode, 16, "coop");
							else
								strcpy_s(mode, 16, "versus");
							VirtualProtect(mode, sizeof(char) * 16, oldProtect, &oldProtect);
						}
						else
							printf("VirtualProtect 0x%X Fail!\n", (DWORD)mode);

						Interfaces.Engine->ClientCmd("echo \"mp_gamemode set %s\"", mode);
					}
#ifdef USE_CVAR_CHANGE
				}
#endif

				// std::this_thread::sleep_for(std::chrono::seconds(1));
			}

			// 修改 sv_cheats 解除更改 cvar 限制
			if (GetAsyncKeyState(VK_APPS) & 0x01)
			{
#ifdef USE_CVAR_CHANGE
				CVAR_MAKE_FLAGS("sv_cheats");
#endif

#ifdef USE_CVAR_CHANGE
				if (gConVar["sv_cheats"] != nullptr)
				{
					gConVar["sv_cheats"]->SetValue(1);
					gConVar["sv_cheats"]->m_fValue = 1.0f;
					gConVar["sv_cheats"]->m_nValue = 1;
					Interfaces.Engine->ClientCmd("echo \"[ConVar] sv_cheats set %d\"",
						gConVar["sv_cheats"]->GetInt());
				}
#endif

				if (Utils::readMemory<int>(gModuleEngine + sv_cheats) != 1)
				{
					Utils::writeMemory(1, gModuleEngine + sv_cheats);
					Interfaces.Engine->ClientCmd("echo \"sv_cheats set %d\"",
						Utils::readMemory<int>(gModuleEngine + sv_cheats));
				}

				// std::this_thread::sleep_for(std::chrono::seconds(1));
			}

			// 切换第三人称
			if (GetAsyncKeyState(VK_NEXT) & 0x01)
				thirdPerson();

			// 显示全部玩家
			if (GetAsyncKeyState(VK_CAPITAL) & 0x01)
				showSpectator();

			// 打开/关闭 自动连跳的自动保持速度
			if (GetAsyncKeyState(VK_F8) & 0x01)
			{
				bAutoStrafe = !bAutoStrafe;
				Interfaces.Engine->ClientCmd("echo \"[segt] auto strafe set %s\"",
					(bAutoStrafe ? "enable" : "disabled"));

				// std::this_thread::sleep_for(std::chrono::seconds(1));
			}

			// 打开/关闭 自动开枪
			if (GetAsyncKeyState(VK_F9) & 0x01)
			{
				bTriggerBot = !bTriggerBot;
				Interfaces.Engine->ClientCmd("echo \"[segt] trigger bot set %s\"",
					(bTriggerBot ? "enable" : "disabled"));

				// std::this_thread::sleep_for(std::chrono::seconds(1));
			}

			// 打开/关闭 自动瞄准
			if (GetAsyncKeyState(VK_F10) & 0x01)
			{
				bAimBot = !bAimBot;
				Interfaces.Engine->ClientCmd("echo \"[segt] aim bot set %s\"",
					(bAimBot ? "enable" : "disabled"));

				// std::this_thread::sleep_for(std::chrono::seconds(1));
			}

			// 打开/关闭 空格自动连跳
			if (GetAsyncKeyState(VK_F11) & 0x01)
			{
				bBhop = !bBhop;
				Interfaces.Engine->ClientCmd("echo \"[segt] auto bunnyHop set %s\"",
					(bBhop ? "enable" : "disabled"));

				// std::this_thread::sleep_for(std::chrono::seconds(1));
			}

			// 打开/关闭 手枪连射
			if (GetAsyncKeyState(VK_F12) & 0x01)
			{
				bRapidFire = !bRapidFire;
				Interfaces.Engine->ClientCmd("echo \"[segt] auto pistol fire %s\"",
					(bRapidFire ? "enable" : "disabled"));

				// std::this_thread::sleep_for(std::chrono::seconds(1));
			}

			// 去除 CRC 验证
			// if (Interfaces.Engine->IsConnected())
			{
#ifdef USE_CVAR_CHANGE
				CVAR_MAKE_FLAGS("sv_pure");
				CVAR_MAKE_FLAGS("sv_consistency");
				CVAR_MAKE_FLAGS("c_thirdpersonshoulder");
#endif

#ifdef USE_CVAR_CHANGE
				if (gConVar["sv_pure"] != nullptr && gConVar["sv_pure"]->GetInt() != 0)
					gConVar["sv_pure"]->SetValue(0);
				if (gConVar["sv_consistency"] != nullptr && gConVar["sv_consistency"]->GetInt() != 0)
					gConVar["sv_consistency"]->SetValue(0);
#endif

				if (Utils::readMemory<int>(gModuleEngine + sv_pure) != 0 ||
					Utils::readMemory<int>(gModuleEngine + sv_consistency) != 0)
				{
					Utils::writeMemory(0, gModuleEngine + sv_pure);
					Utils::writeMemory(0, gModuleEngine + sv_consistency);

					Interfaces.Engine->ClientCmd("echo \"sv_pure and sv_consistency set %d\"",
						Utils::readMemory<int>(gModuleEngine + sv_pure));

					// std::this_thread::sleep_for(std::chrono::seconds(1));
				}
			}

			if (!connected)
			{
				Interfaces.Engine->ClientCmd("echo \"********* connected *********\"");

				Utils::log("*** connected ***");
			}

			connected = true;
		}
		else if (connected && !Interfaces.Engine->IsInGame())
		{
			/*
#ifdef USE_CVAR_CHANGE
			if (gConVar["r_drawothermodels"] != nullptr)
			{
				if (gConVar["r_drawothermodels"]->GetInt() != 1)
				{
					gConVar["r_drawothermodels"]->SetValue(1);
				}
			}
			else
			{
#endif
				if (Utils::readMemory<int>(gModuleClient + r_drawothermodels) != 1)
				{
					Utils::writeMemory<int>(1, gModuleClient + r_drawothermodels);
				}
#ifdef USE_CVAR_CHANGE
			}
#endif

#ifdef USE_CVAR_CHANGE
			if (gConVar["cl_drawshadowtexture"] != nullptr)
			{
				if (gConVar["cl_drawshadowtexture"]->GetInt() != 0)
				{
					gConVar["cl_drawshadowtexture"]->SetValue(0);
				}
			}
			else
			{
#endif
				if (Utils::readMemory<int>(gModuleClient + cl_drawshadowtexture) != 0)
				{
					Utils::writeMemory(0, gModuleClient + cl_drawshadowtexture);
				}
#ifdef USE_CVAR_CHANGE
			}
#endif

#ifdef USE_CVAR_CHANGE
			if (gConVar["mat_fullbright"] != nullptr)
			{
				if (gConVar["mat_fullbright"]->GetInt() != 0)
				{
					gConVar["mat_fullbright"]->SetValue(0);
				}
			}
			else
			{
#endif
				if (Utils::readMemory<int>(gModuleMaterial + mat_fullbright) != 0)
				{
					Utils::writeMemory(0, gModuleMaterial + mat_fullbright);
				}
#ifdef USE_CVAR_CHANGE
			}
#endif
			*/

			connected = false;
			Utils::log("*** disconnected ***");
		}

		if (GetAsyncKeyState(VK_ADD) & 0x01)
		{
			Interfaces.Engine->ClientCmd("alias fastmelee_loop \"+attack; slot1; wait 1; -attack; slot2; wait %d; fastmelee_launcher\"", ++fmWait);
			Interfaces.Engine->ClientCmd("echo \"fastmelee wait set %d\"", fmWait);
			// std::this_thread::sleep_for(std::chrono::seconds(1));
		}

		if (GetAsyncKeyState(VK_SUBTRACT) & 0x01)
		{
			Interfaces.Engine->ClientCmd("alias fastmelee_loop \"+attack; slot1; wait 1; -attack; slot2; wait %d; fastmelee_launcher\"", --fmWait);
			Interfaces.Engine->ClientCmd("echo \"fastmelee wait set %d\"", fmWait);
			// std::this_thread::sleep_for(std::chrono::seconds(1));
		}

		if (GetAsyncKeyState(VK_END) & 0x01)
		{
			errlog.close();
			ExitProcess(0);
		}

		if (GetAsyncKeyState(VK_DELETE) & 0x01)
			Interfaces.Engine->ClientCmd("disconnect");
	}
}

int __stdcall Hooked_InKeyEvent(int eventcode, ButtonCode_t keynum, const char *pszCurrentBinding)
{
	return oInKeyEvent(eventcode, keynum, pszCurrentBinding);
}

void __stdcall Hooked_RunCommand(CBaseEntity* pEntity, CUserCmd* pCmd, CMoveHelper* moveHelper)
{
	oRunCommand(pEntity, pCmd, moveHelper);

	if (Interfaces.MoveHelper == nullptr)
		Utils::log("MoveHelperPointer = 0x%X", (DWORD)moveHelper);

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
		std::cout << "Hooked_Reset trigged." << std::endl;

		if ((DWORD)dh::gDeviceInternal == (DWORD)device)
			Utils::log("Hooked_Reset success");
	}

	if (dh::gDeviceInternal == nullptr)
	{
		ResetDeviceHook(device);
		showHint = true;
	}

	if (!bImGuiInitialized)
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
		std::cout << "Hooked_EndScene trigged." << std::endl;

		if ((DWORD)dh::gDeviceInternal == (DWORD)device)
			Utils::log("Hooked_EndScene success");
	}

	if (dh::gDeviceInternal == nullptr)
	{
		ResetDeviceHook(device);
		showHint = true;
	}

	if (!bImGuiInitialized)
	{
		// 鍒濆鍖
		// ImGui_ImplDX9_Init(FindWindowA(NULL, "Left 4 Dead 2"), device);
		drawRender = new DrawManager(device);
		bImGuiInitialized = true;
	}
	else
	{
		// ImGui::GetIO().MouseDrawCursor = !Interfaces.Engine->IsInGame();

		// ImGui_ImplDX9_NewFrame();
		// drawRender->BeginRendering();

		// CBaseEntity* local = GetLocalClient();
		/*
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
		// 鐜╁
		if (!entity->IsAlive() || entity->GetHealth() <= 0)
		continue;

		Vector head, foot;
		D3DCOLOR color = getPlayerColor(local, entity);

		if (WorldToScreen(entity->GetEyePosition(), head) &&
		WorldToScreen(entity->GetAbsOrigin(), foot))
		{
		// 鏄剧ず绫诲瀷
		drawRender->RenderText(color, head.x, head.y, true, "[%d] %s",
		entity->GetHealth(), GetZombieClassName(entity).c_str());

		static bool showHint = true;
		if (showHint)
		{
		Utils::log("zombieClass Draw: %s", GetZombieClassName(entity).c_str());
		showHint = false;
		}

		float height = fabs(head.y - foot.y);
		float width = height * 0.65f;

		// 缁樺埗涓�涓
		drawRender->RenderRect(color, foot.x - width / 2, foot.y, width, -height);
		// drawRender->DrawRect(foot.x - width / 2, foot.y, width, -height, color);
		}
		}
		else
		{
		// 鏅劅
		// TODO: 濡傛灉杩欎釜瀹炰綋鏄櫘鎰燂紝灏辩粯鍒跺嚭鏉
		}
		}
		}
		*/

		// ImGui::Render();
		// drawRender->EndRendering();
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
		// std::cout << "Hooked_DrawIndexedPrimitive trigged." << std::endl;
		if ((DWORD)dh::gDeviceInternal == (DWORD)device)
			Utils::log("Hooked_DrawIndexedPrimitive success");
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
		// std::cout << "Hooked_CreateQuery trigged." << std::endl;
		if ((DWORD)dh::gDeviceInternal == (DWORD)device)
			Utils::log("Hooked_CreateQuery success");
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
		// std::cout << "Hooked_Present trigged." << std::endl;
		if ((DWORD)dh::gDeviceInternal == (DWORD)device)
			Utils::log("Hooked_Present success");
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
					Interfaces.Engine->ClientCmd("-jump");
					std::this_thread::sleep_for(std::chrono::microseconds(1));
				}

				Interfaces.Engine->ClientCmd("+jump");
				repeat = true;
			}
			else if (flags == FL_CLIENT || flags == (FL_DUCKING | FL_CLIENT) || flags == (FL_INWATER | FL_CLIENT) ||
				flags == (FL_DUCKING | FL_CLIENT | FL_INWATER))
			{
				Interfaces.Engine->ClientCmd("-jump");
				if (repeat)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(16));
					Interfaces.Engine->ClientCmd("+jump");
					std::this_thread::sleep_for(std::chrono::microseconds(1));
					Interfaces.Engine->ClientCmd("-jump");
					repeat = false;
				}
			}
		}

		std::this_thread::sleep_for(std::chrono::microseconds(1));
	}
}

void autoPistol()
{
	CBaseEntity* client = nullptr;

	for (;;)
	{
		client = GetLocalClient();
		if (bRapidFire && client && Interfaces.Engine->IsInGame() && !Interfaces.Engine->IsConsoleVisible() &&
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
					// 妫�鏌ュ綋鍓嶆鍣ㄦ槸鍚﹂渶瑕佽繛鐐规墠鑳芥寔缁紑鏋
					// 鍥犱负杩欎釜杩炵偣寮�鏋晥鐜囧苟涓嶅ソ锛屽浜庤嚜鍔ㄦ鍣ㄦ潵璇达紝浼氶檷浣庡皠鍑婚�熷害
					// 杩炲彂闇板脊鏋繛鐐瑰彲浠ュ姞蹇皠閫
					if (IsWeaponSingle(weaponId))
					{
						Interfaces.Engine->ClientCmd("+attack");
						std::this_thread::sleep_for(std::chrono::milliseconds(9));
						Interfaces.Engine->ClientCmd("-attack");
					}
				}
				else
				{
					// 褰撳墠鐬勫噯鐨勬槸闃熷弸锛屽苟涓旈槦鍙嬩笉闇�瑕佸府鍔
					// 鍦ㄨ繖閲屽仠姝㈠紑鏋紝闃叉闃熷弸浼ゅ
					Interfaces.Engine->ClientCmd("-attack");

					// printf("current weapon id = %d\n", weaponId);
				}
			}
		}

		std::this_thread::sleep_for(std::chrono::microseconds(1));
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
				// 褰撳墠浣嶇疆
				Vector myOrigin = client->GetEyePosition();

				// 妫�鏌ユ槸鍚﹂渶瑕侀�夋嫨鏂扮殑鏁屼汉
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
						if (target == nullptr || target->IsDormant() || target->GetTeam() == team ||
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

						// 閫夋嫨鏈�杩戠殑鏁屼汉
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
					// 鐩爣浣嶇疆
					Vector position;
					try
					{
						// 鏍规嵁澶撮儴楠ㄥご鏉ョ瀯鍑嗭紝杩欑帺鎰忓緢涓嶇ǔ瀹氾紝鏃朵笉鏃朵細 boom
						// 濂藉鏄瀯寰楀噯锛屼笉浼氳鍏朵粬鍥犳暟褰卞搷
						position = GetHeadPosition(pCurrentAiming);
					}
					catch (...)
					{
						// 鑾峰彇楠ㄩ浣嶇疆澶辫触
						position = pCurrentAiming->GetEyePosition();
						Utils::log("CBasePlayer::SetupBone error");

						// 鏍规嵁涓嶅悓鐨勬儏鍐电‘瀹氶珮搴
						int zombieClass = pCurrentAiming->GetNetProp<int>("m_zombieClass", "DT_TerrorPlayer");
						if (zombieClass == ZC_JOCKEY)
							position.z = pCurrentAiming->GetAbsOrigin().z + 30.0f;
						else if (zombieClass == ZC_HUNTER && (pCurrentAiming->GetFlags() & FL_DUCKING))
							position.z -= 12.0f;
					}

					// 閫熷害棰勬祴锛屼細瀵艰嚧灞忓箷鏅冨姩锛屾墍浠ヤ笉闇�瑕
					// position += (pCurrentAiming->GetVelocity() * Interfaces.Globals->interval_per_tick);

					Interfaces.Engine->SetViewAngles(CalculateAim(myOrigin, position));
				}
			}
			else if (pCurrentAiming != nullptr)
			{
				pCurrentAiming = nullptr;
				Interfaces.Engine->ClientCmd("echo \"*** auto aim stopped ***\"");
			}
		}

		mAimbot.unlock();
		std::this_thread::sleep_for(std::chrono::microseconds(1));
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

			// 娌℃湁浠讳綍鏁堟灉...
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

		std::this_thread::sleep_for(std::chrono::microseconds(1));
	}

	std::this_thread::sleep_for(std::chrono::microseconds(1));
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
			Interfaces.Engine->ClientCmd("+attack");
			std::this_thread::sleep_for(std::chrono::milliseconds(9));
			Interfaces.Engine->ClientCmd("-attack");
			Interfaces.Engine->ClientCmd("slot1");
			std::this_thread::sleep_for(std::chrono::milliseconds(9));
			Interfaces.Engine->ClientCmd("slot2");
			tookoutTime = GetServerTime() + 0.5f;
		}
	}
	else
		tookoutTime = 0.0f;

	std::this_thread::sleep_for(std::chrono::microseconds(1));
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
		// 鍒囨崲鍒扮涓変汉绉
		local->SetNetProp<int>("m_hObserverTarget", 0, "DT_BasePlayer");
		local->SetNetProp<int>("m_iObserverMode", 1, "DT_BasePlayer");
		local->SetNetProp<int>("m_bDrawViewmodel", 0, "DT_BasePlayer");
		}
		else if (local->GetNetProp<int>("m_hObserverTarget", "DT_BasePlayer") == 0)
		{
		// 鍒囨崲鍒扮涓�浜虹О
		local->SetNetProp<int>("m_hObserverTarget", -1, "DT_BasePlayer");
		local->SetNetProp<int>("m_iObserverMode", 0, "DT_BasePlayer");
		local->SetNetProp<int>("m_bDrawViewmodel", 1, "DT_BasePlayer");
		}
		*/
	}

	std::this_thread::sleep_for(std::chrono::seconds(1));
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

		Interfaces.Engine->ClientCmd("echo \"========= player list =========\"");

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
				// 瑙傚療鑰
				if (mode != 4 && mode != 5)
					continue;

				target = (CBaseEntity*)player->GetNetProp<void*>("m_iObserverMode", "DT_BasePlayer");
				if (target && (int)target != -1)
					target = Interfaces.ClientEntList->GetClientEntityFromHandle(target);

				if (target == nullptr || (int)target == -1)
					continue;

				if ((DWORD)target == (DWORD)local)
				{
					// 姝ｅ湪瑙傚療鏈湴鐜╁
#ifdef USE_PLAYER_INFO
					Interfaces.Engine->ClientCmd("echo \"[spectator] player %s %s you\"",
						info.name, (mode == 4 ? "watching" : "following"));
#else
					Interfaces.Engine->ClientCmd("echo \"[spectator] player %d %s you\"",
						player->GetIndex(), (mode == 4 ? "watching" : "following"));
#endif
				}
				else
				{
					// 姝ｅ湪瑙傚療鍏朵粬鐜╁
#ifdef USE_PLAYER_INFO
					player_info_t other;
					Interfaces.Engine->GetPlayerInfo(target->GetIndex(), &other);
					Interfaces.Engine->ClientCmd("echo \"[spectator] player %s %s %s\"",
						info.name, (mode == 4 ? "watching" : "following"), other.name);
#else
					Interfaces.Engine->ClientCmd("echo \"[spectator] player %d %s %d\"",
						player->GetIndex(), (mode == 4 ? "watching" : "following"), target->GetIndex());
#endif
				}
			}
			else if (team == 2)
			{
				// 鐢熻繕鑰
				if (player->IsAlive())
				{
					// 娲荤潃
#ifdef USE_PLAYER_INFO
					Interfaces.Engine->ClientCmd("echo \"[survivor] player %s is alive (%d + %.0f)\"",
						info.name, player->GetHealth(), player->GetNetProp<float>("m_healthBuffer", "DT_TerrorPlayer"));
#else
					Interfaces.Engine->ClientCmd("echo \"[survivor] player %d is alive (%d + %.0f)\"",
						player->GetIndex(), player->GetHealth(), player->GetNetProp<float>("m_healthBuffer", "DT_TerrorPlayer"));
#endif
				}
				else
				{
					// 姝讳骸
					if (mode != 4 && mode != 5)
						continue;

					target = (CBaseEntity*)player->GetNetProp<void*>("m_iObserverMode", "DT_BasePlayer");
					if (target && (int)target != -1)
						target = Interfaces.ClientEntList->GetClientEntityFromHandle(target);

					if (target == nullptr || (int)target == -1)
						continue;

					if ((DWORD)target == (DWORD)local)
					{
						// 姝ｅ湪瑙傚療鏈湴鐜╁
#ifdef USE_PLAYER_INFO
						Interfaces.Engine->ClientCmd("echo \"[survivor] player %s %s you\"",
							info.name, (mode == 4 ? "watching" : "following"));
#else
						Interfaces.Engine->ClientCmd("echo \"[survivor] player %d %s you\"",
							player->GetIndex(), (mode == 4 ? "watching" : "following"));
#endif
					}
					else
					{
						// 姝ｅ湪瑙傚療鍏朵粬鐜╁
#ifdef USE_PLAYER_INFO
						player_info_t other;
						Interfaces.Engine->GetPlayerInfo(target->GetIndex(), &other);
						Interfaces.Engine->ClientCmd("echo \"[survivor] player %s %s %s\"",
							info.name, (mode == 4 ? "watching" : "following"), other.name);
#else
						Interfaces.Engine->ClientCmd("echo \"[survivor] player %d %s %d\"",
							player->GetIndex(), (mode == 4 ? "watching" : "following"), target->GetIndex());
#endif
					}
				}
			}
			else if (team == 3)
			{
				// 鎰熸煋鑰
				int zombie = player->GetNetProp<int>("m_zombieClass", "DT_TerrorPlayer");
				char zombieName[32];
				switch (zombie)
				{
				case ZC_SMOKER:
					strcpy_s(zombieName, "smoker");
					break;
				case ZC_BOOMER:
					strcpy_s(zombieName, "boomer");
					break;
				case ZC_HUNTER:
					strcpy_s(zombieName, "hunter");
					break;
				case ZC_SPITTER:
					strcpy_s(zombieName, "spitter");
					break;
				case ZC_JOCKEY:
					strcpy_s(zombieName, "jockey");
					break;
				case ZC_CHARGER:
					strcpy_s(zombieName, "charger");
					break;
				case ZC_TANK:
					strcpy_s(zombieName, "tank");
					break;
				default:
					ZeroMemory(zombieName, 32);
				}

				if (player->IsAlive())
				{
					// 娲荤潃
#ifdef USE_PLAYER_INFO
					Interfaces.Engine->ClientCmd("echo \"[infected] player %s is %s (%d)\"",
						info.name, zombieName, player->GetHealth());
#else
					Interfaces.Engine->ClientCmd("echo \"[infected] player %d is %s (%d)\"",
						player->GetIndex(), zombieName, player->GetHealth());
#endif
				}
				else if (player->GetNetProp<byte>("m_isGhost", "DT_TerrorPlayer"))
				{
					// 骞界伒鐘舵�
#ifdef USE_PLAYER_INFO
					Interfaces.Engine->ClientCmd("echo \"[infected] player %s is ghost %s (%d)\"",
						info.name, zombieName, player->GetHealth());
#else
					Interfaces.Engine->ClientCmd("echo \"[infected] player %d is ghost %s (%d)\"",
						player->GetIndex(), zombieName, player->GetHealth());
#endif
				}
				else
				{
					// 姝讳骸
					if (mode != 4 && mode != 5)
						continue;

					target = (CBaseEntity*)player->GetNetProp<void*>("m_iObserverMode", "DT_BasePlayer");
					if (target && (int)target != -1)
						target = Interfaces.ClientEntList->GetClientEntityFromHandle(target);

					if (target == nullptr || (int)target == -1)
						continue;

					if ((DWORD)target == (DWORD)local)
					{
						// 姝ｅ湪瑙傚療鏈湴鐜╁
#ifdef USE_PLAYER_INFO
						Interfaces.Engine->ClientCmd("echo \"[infected] player %s %s you\"",
							info.name, (mode == 4 ? "watching" : "following"));
#else
						Interfaces.Engine->ClientCmd("echo \"[infected] player %s %s you\"",
							player->GetIndex(), (mode == 4 ? "watching" : "following"));
#endif
					}
					else
					{
						// 姝ｅ湪瑙傚療鍏朵粬鐜╁
#ifdef USE_PLAYER_INFO
						player_info_t other;
						Interfaces.Engine->GetPlayerInfo(target->GetIndex(), &other);
						Interfaces.Engine->ClientCmd("echo \"[infected] player %s %s %s\"",
							info.name, (mode == 4 ? "watching" : "following"), other.name);
#else
						Interfaces.Engine->ClientCmd("echo \"[infected] player %d %s %d\"",
							player->GetIndex(), (mode == 4 ? "watching" : "following"), target->GetIndex());
#endif
					}
				}
			}
		}

		Interfaces.Engine->ClientCmd("echo \"========= list end =========\"");
	}

	std::this_thread::sleep_for(std::chrono::microseconds(1));
}

void bindAlias(int wait)
{
	Interfaces.Engine->ClientCmd("echo \"echo \"========= alias begin =========\"\"");
	Interfaces.Engine->ClientCmd("alias +autofire \"alias autofire_launcher autofire_loop; autofire_launcher\"");
	Interfaces.Engine->ClientCmd("alias -autofire \"alias autofire_launcher autofire_stop\"");
	Interfaces.Engine->ClientCmd("alias autofire_launcher autofire_loop");
	Interfaces.Engine->ClientCmd("alias autofire_loop \"+attack; wait 5; -attack; wait 5; autofire_launcher\"");
	Interfaces.Engine->ClientCmd("alias autofire_stop \"-attack\"");
	Interfaces.Engine->ClientCmd("alias +autojump \"alias autojump_launcher autojump_loop; autojump_launcher\"");
	Interfaces.Engine->ClientCmd("alias -autojump \"alias autojump_launcher autojump_stop\"");
	Interfaces.Engine->ClientCmd("alias autojump_launcher autojump_loop");
	Interfaces.Engine->ClientCmd("alias autojump_loop \"+jump; wait 5; -jump; wait 5; autojump_launcher\"");
	Interfaces.Engine->ClientCmd("alias autojump_stop \"-jump\"");
	Interfaces.Engine->ClientCmd("alias +bigjump \"+jump; +duck\"");
	Interfaces.Engine->ClientCmd("alias -bigjump \"-jump; -duck\"");
	Interfaces.Engine->ClientCmd("alias +pistolspam \"alias pistolspam dopistolspam; dopistolspam\"");
	Interfaces.Engine->ClientCmd("alias -pistolspam \"alias pistolspam -use\"");
	Interfaces.Engine->ClientCmd("alias dopistolspam \"+use; wait 3; -use; wait 3; pistolspam\"");
	Interfaces.Engine->ClientCmd("alias +fastmelee \"alias fastmelee_launcher fastmelee_loop; fastmelee_launcher\"");
	Interfaces.Engine->ClientCmd("alias fastmelee_launcher fastmelee_loop");
	Interfaces.Engine->ClientCmd("alias fastmelee_loop \"+attack; slot1; wait 1; -attack; slot2; wait 45; fastmelee_launcher\"");
	Interfaces.Engine->ClientCmd("alias fastmelee_stop \"-attack\"");
	Interfaces.Engine->ClientCmd("alias -fastmelee \"alias fastmelee_launcher fastmelee_stop\"");
	Interfaces.Engine->ClientCmd("alias thirdperson_toggle \"thirdperson_enable\"");
	Interfaces.Engine->ClientCmd("alias thirdperson_enable \"alias thirdperson_toggle thirdperson_disable; thirdpersonshoulder\"");
	Interfaces.Engine->ClientCmd("alias thirdperson_disable \"alias thirdperson_toggle thirdperson_enable; thirdpersonshoulder; c_thirdpersonshoulder 0\"");
	Interfaces.Engine->ClientCmd("c_thirdpersonshoulderoffset 0");
	Interfaces.Engine->ClientCmd("c_thirdpersonshoulderaimdist 720");
	Interfaces.Engine->ClientCmd("cam_ideallag 0");
	Interfaces.Engine->ClientCmd("cam_idealdist 40");
	Interfaces.Engine->ClientCmd("bind leftarrow \"incrementvar cam_idealdist 30 130 10\"");
	Interfaces.Engine->ClientCmd("bind rightarrow \"incrementvar cam_idealdist 30 130 -10\"");
	Interfaces.Engine->ClientCmd("bind uparrow \"incrementvar c_thirdpersonshoulderheight 5 25 5\"");
	Interfaces.Engine->ClientCmd("bind downarrow \"incrementvar c_thirdpersonshoulderheight 5 25 -5\"");
	Interfaces.Engine->ClientCmd("cl_crosshair_alpha 255");
	Interfaces.Engine->ClientCmd("cl_crosshair_blue 0");
	Interfaces.Engine->ClientCmd("cl_crosshair_green 50");
	Interfaces.Engine->ClientCmd("cl_crosshair_red 190");
	Interfaces.Engine->ClientCmd("cl_crosshair_dynamic 0");
	Interfaces.Engine->ClientCmd("cl_crosshair_thickness 1.0");
	Interfaces.Engine->ClientCmd("crosshair 1");
	Interfaces.Engine->ClientCmd("con_enable 1");
	Interfaces.Engine->ClientCmd("muzzleflash_light 1");
	Interfaces.Engine->ClientCmd("sv_voiceenable 1");
	Interfaces.Engine->ClientCmd("voice_enable 1");
	Interfaces.Engine->ClientCmd("voice_forcemicrecord 1");
	Interfaces.Engine->ClientCmd("mat_monitorgamma 1.6");
	Interfaces.Engine->ClientCmd("mat_monitorgamma_tv_enabled 1");
	Interfaces.Engine->ClientCmd("z_nightvision_r 255");
	Interfaces.Engine->ClientCmd("z_nightvision_g 255");
	Interfaces.Engine->ClientCmd("z_nightvision_b 255");
	Interfaces.Engine->ClientCmd("bind space +jump");
	Interfaces.Engine->ClientCmd("bind mouse1 +attack");
	Interfaces.Engine->ClientCmd("bind mouse2 +attack2");
	Interfaces.Engine->ClientCmd("bind mouse3 +zoom");
	Interfaces.Engine->ClientCmd("bind ctrl +duck");
	Interfaces.Engine->ClientCmd("unbind ins");
	Interfaces.Engine->ClientCmd("unbind home");
	Interfaces.Engine->ClientCmd("unbind pgup");
	Interfaces.Engine->ClientCmd("unbind pgdn");
	Interfaces.Engine->ClientCmd("unbind end");
	Interfaces.Engine->ClientCmd("unbind del");
	Interfaces.Engine->ClientCmd("unbind f9");
	Interfaces.Engine->ClientCmd("unbind f10");
	Interfaces.Engine->ClientCmd("unbind f11");
	Interfaces.Engine->ClientCmd("unbind f12");
	Interfaces.Engine->ClientCmd("unbind f8");
	Interfaces.Engine->ClientCmd("echo \"echo \"========= alias end =========\"\"");
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

	std::this_thread::sleep_for(std::chrono::microseconds(1));
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
				Interfaces.Engine->ClientCmd("+attack");
				std::this_thread::sleep_for(std::chrono::milliseconds(9));
				Interfaces.Engine->ClientCmd("-attack");
			}
		}

		std::this_thread::sleep_for(std::chrono::microseconds(1));
	}
}
