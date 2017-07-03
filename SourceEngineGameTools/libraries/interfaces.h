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
		Engine = (CEngine*)GetPointer("engine.dll", "VEngineClient");
		Trace = (CTrace*)GetPointer("engine.dll", "EngineTraceClient");
		Client = (CClient*)GetPointer("client.dll", "VClient");
		ClientEntList = (CClientEntityList*)GetPointer("client.dll", "VClientEntityList");
		ModelInfo = (CModelInfo*)GetPointer("engine.dll", "VModelInfoClient");
		Panel = (CPanel*)GetPointer("vgui2.dll", "VGUI_Panel");
		Surface = (CSurface*)GetPointer("vguimatsurface.dll", "VGUI_Surface");
		PlayerInfo = (CPlayerInfoManager*)GetPointer("server.dll", "PlayerInfoManager");
		Prediction = (CPrediction*)GetPointer("client.dll", "VClientPrediction");
		GameMovement = (CGameMovement*)GetPointer("client.dll", "GameMovement");
		DebugOverlay = (CDebugOverlay*)GetPointer("engine.dll", "VDebugOverlay");
		GameEvent = (CGameEvent*)GetPointer("engine.dll", "GAMEEVENTSMANAGER");
		ModelRender = (CModelRender*)GetPointer("engine.dll", "VEngineModel");
		RenderView = (CRenderView*)GetPointer("engine.dll", "VEngineRenderView");
		Cvar = (ICvar*)GetPointer("vstdlib.dll", "VEngineCvar");
		
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