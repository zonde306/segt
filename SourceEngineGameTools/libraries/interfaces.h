#pragma once
#include <memory>

class CClientEntityList;
class CTrace;
class ICvar;
class CDebugOverlay;
class CGameEvent;
class CModelRender;
class CRenderView;
class CMoveHelper;

class CInterfaces
{
public:
	CEngine *Engine;
	CClient *Client;
	CInput *Input;
	CClientEntityList *ClientEntList;
	CModelInfo *ModelInfo;
	CPanel* Panel;
	CSurface* Surface;
	CTrace* Trace;
	CGlobalVarsBase* Globals;

	CPlayerInfoManager* PlayerInfo;
	CPrediction* Prediction;
	CGameMovement* GameMovement;
	CDebugOverlay* DebugOverlay;
	CGameEvent* GameEvent;
	CModelRender* ModelRender;
	CRenderView* RenderView;
	CMoveHelper* MoveHelper;
	ICvar* Cvar;

	void* ClientMode;
	CVMTHookManager* ClientModeHook;
	CVMTHookManager* PanelHook;
	CVMTHookManager* ClientHook;
	CVMTHookManager* PredictionHook;
	CVMTHookManager* ModelRenderHook;

	void GetInterfaces()
	{
		Engine = (CEngine*)GetPointer(XorStr("engine.dll"), XorStr("VEngineClient"));
		Trace = (CTrace*)GetPointer(XorStr("engine.dll"), XorStr("EngineTraceClient"));
		Client = (CClient*)GetPointer(XorStr("client.dll"), XorStr("VClient"));
		ClientEntList = (CClientEntityList*)GetPointer(XorStr("client.dll"), XorStr("VClientEntityList"));
		ModelInfo = (CModelInfo*)GetPointer(XorStr("engine.dll"), XorStr("VModelInfoClient"));
		Panel = (CPanel*)GetPointer(XorStr("vgui2.dll"), XorStr("VGUI_Panel"));
		Surface = (CSurface*)GetPointer(XorStr("vguimatsurface.dll"), XorStr("VGUI_Surface"));
		PlayerInfo = (CPlayerInfoManager*)GetPointer(XorStr("server.dll"), XorStr("PlayerInfoManager"));
		Prediction = (CPrediction*)GetPointer(XorStr("client.dll"), XorStr("VClientPrediction"));
		GameMovement = (CGameMovement*)GetPointer(XorStr("client.dll"), XorStr("GameMovement"));
		DebugOverlay = (CDebugOverlay*)GetPointer(XorStr("engine.dll"), XorStr("VDebugOverlay"));
		GameEvent = (CGameEvent*)GetPointer(XorStr("engine.dll"), XorStr("GAMEEVENTSMANAGER"));
		ModelRender = (CModelRender*)GetPointer(XorStr("engine.dll"), XorStr("VEngineModel"));
		RenderView = (CRenderView*)GetPointer(XorStr("engine.dll"), XorStr("VEngineRenderView"));
		Cvar = (ICvar*)GetPointer(XorStr("vstdlib.dll"), XorStr("VEngineCvar"));
		
		PDWORD pdwClient = *(PDWORD*)Client;
		// Input = *(CInput**)((*(DWORD**)Client)[15] + 0x1);

		Input = (CInput*)*(PDWORD**)*(PDWORD**)(pdwClient[indexes::CreateMove] + 0x28);

		DWORD dwInitAddr = (DWORD)(pdwClient[0]);
		for (DWORD dwIter = 0; dwIter <= 0xFF; dwIter++)
		{
			if (*(PBYTE)(dwInitAddr + dwIter) == 0xA3)
			{
				Globals = (CGlobalVarsBase*)*(PDWORD)*(PDWORD)(dwInitAddr + dwIter + 1);
				// printf("Globals Found: 0x%X || 0x%X\n", (DWORD)Globals, ((DWORD)Globals - (DWORD)GetModuleHandleA("client.dll")));
				printo("Globals Variable", Globals);
				break;
			}
		}
		
		// printf("Input Found: 0x%X\n", (DWORD)Input);
		printo("Input", Input);

		/*
		void** pClientTable = *reinterpret_cast<void***>(Client);
		ClientMode = **reinterpret_cast<void***>(reinterpret_cast<DWORD>(pClientTable[10]) + 5);
		*/

		PanelHook = new CVMTHookManager(Panel);
		ClientHook = new CVMTHookManager(Client);
		PredictionHook = new CVMTHookManager(Prediction);
		ModelRenderHook = new CVMTHookManager(ModelRender);
	}

	void* GetPointer(const char* Module, const char* InterfaceName)
	{
		void* Interface = NULL;
		char PossibleInterfaceName[1024];

		CreateInterfaceFn CreateInterface = (CreateInterfaceFn)GetProcAddress(GetModuleHandleA(Module), "CreateInterface");
		for (int i = 1; i < 100; i++)
		{
			sprintf_s(PossibleInterfaceName, "%s0%i", InterfaceName, i);
			Interface = (void*)CreateInterface(PossibleInterfaceName, NULL);
			if (Interface != NULL)
			{
				printf("%s Found: 0x%X\n", PossibleInterfaceName, (DWORD)Interface);
				break;
			}
			sprintf_s(PossibleInterfaceName, "%s00%i", InterfaceName, i);
			Interface = (void*)CreateInterface(PossibleInterfaceName, NULL);
			if (Interface != NULL)
			{
				printf("%s Found: 0x%X\n", PossibleInterfaceName, (DWORD)Interface);				
				break;
			}
		}

		return Interface;
	}
};

CInterfaces Interfaces;