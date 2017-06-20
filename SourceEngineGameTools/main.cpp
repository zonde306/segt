#include "main.h"

#define GetLocalClient()	Interfaces.ClientEntList->GetClientEntity(Interfaces.Engine->GetLocalPlayer())
void StartCheat();

BOOL WINAPI DllMain(HINSTANCE Instance, DWORD Reason, LPVOID Reserved)
{
	if (Reason == DLL_PROCESS_ATTACH)
	{
		AllocConsole();

		HWND hwnd = GetConsoleWindow();

		HMENU hMenu = GetSystemMenu(hwnd, FALSE);
		if (hMenu) DeleteMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);

		SetConsoleTitleA("Console");
		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);
		// printf("RUN\n");
		Interfaces.GetInterfaces();
		netVars = new CNetVars();

		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)StartCheat, NULL, NULL, NULL);
	}

	return TRUE;
}

typedef void(__fastcall* FnPaintTraverse)(void*, unsigned int, bool, bool);
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

static CBaseEntity* pCurrentAiming;
DetourXS dDrawIndexedPrimitive, dEndScene, dReset, dCreateQuery, dPresent;

void bunnyHop();
void autoPistol();
void autoAim();
void esp();
void bindAlias(int);
void pure(void*);

void StartCheat()
{
	// GetClientModeNormal B8 ? ? ? ? C3
	typedef void*(__stdcall* FnGetClientMode)();
	FnGetClientMode GetClientModeNormal = nullptr;
	DWORD size, address;
	address = Utils::GetModuleBase("client.dll", &size);
	printf("client.dll = 0x%X\n", address);

	if ((GetClientModeNormal = (FnGetClientMode)Utils::FindPattern(address, size, "B8 ? ? ? ? C3")) != nullptr)
	{
		Interfaces.ClientMode = GetClientModeNormal();
		if (Interfaces.ClientMode)
		{
			Interfaces.ClientModeHook = new CVMTHookManager(Interfaces.ClientMode);
			printf("ClientModePtr = 0x%X\n", (DWORD)Interfaces.ClientMode);
		}
	}
	else
	{
		Interfaces.ClientMode = *(void**)(address + IClientModePointer);
		if (Interfaces.ClientMode && (DWORD)Interfaces.ClientMode > address)
		{
			Interfaces.ClientModeHook = new CVMTHookManager((void*)(*(DWORD*)Interfaces.ClientMode));
			printf("ClientModePtr = 0x%X\n", (DWORD)Interfaces.ClientMode);
		}
	}
	
	if (Interfaces.PanelHook && indexes::PaintTraverse > -1)
	{
		oPaintTraverse = (FnPaintTraverse)Interfaces.PanelHook->HookFunction(indexes::PaintTraverse, Hooked_PaintTraverse);
		printf("oPaintTraverse = 0x%X\n", (DWORD)oPaintTraverse);
	}

	if (Interfaces.ClientModeHook && indexes::SharedCreateMove > -1)
	{
		oCreateMoveShared = (FnCreateMoveShared)Interfaces.ClientModeHook->HookFunction(indexes::SharedCreateMove, Hooked_CreateMoveShared);
		printf("oCreateMoveShared = 0x%X\n", (DWORD)oCreateMoveShared);
	}

	if (Interfaces.ClientHook && indexes::CreateMove > -1)
	{
		oCreateMove = (FnCreateMove)Interfaces.ClientHook->HookFunction(indexes::CreateMove, Hooked_CreateMove);
		printf("oCreateMove = 0x%X\n", (DWORD)oCreateMove);
	}

	if (Interfaces.ClientHook && indexes::FrameStageNotify > -1)
	{
		oFrameStageNotify = (FnFrameStageNotify)Interfaces.ClientHook->HookFunction(indexes::FrameStageNotify, Hooked_FrameStageNotify);
		printf("oFrameStageNotify = 0x%X\n", (DWORD)oFrameStageNotify);
	}

	if (Interfaces.ClientHook && indexes::InKeyEvent > -1)
	{
		oInKeyEvent = (FnInKeyEvent)Interfaces.ClientHook->HookFunction(indexes::InKeyEvent, Hooked_InKeyEvent);
		printf("oInKeyEvent = 0x%X\n", (DWORD)oInKeyEvent);
	}

	if (Interfaces.PredictionHook && indexes::RunCommand > -1)
	{
		oRunCommand = (FnRunCommand)Interfaces.ClientHook->HookFunction(indexes::RunCommand, Hooked_RunCommand);
		printf("oRunCommand = 0x%X\n", (DWORD)oRunCommand);
	}

	if (Interfaces.ModelRenderHook && indexes::DrawModel > -1)
	{
		oDrawModel = (FnDrawModel)Interfaces.ClientHook->HookFunction(indexes::DrawModel, Hooked_DrawModel);
		printf("oDrawModel = 0x%X\n", (DWORD)oDrawModel);
	}

	if (Interfaces.Cvar)
	{
		printf("sv_cheats = 0x%X\n", (DWORD)Interfaces.Cvar->FindVar("sv_cheats"));
		printf("r_drawothermodels = 0x%X\n", (DWORD)Interfaces.Cvar->FindVar("r_drawothermodels"));
		printf("cl_drawshadowtexture = 0x%X\n", (DWORD)Interfaces.Cvar->FindVar("cl_drawshadowtexture"));
		printf("mat_fullbright = 0x%X\n", (DWORD)Interfaces.Cvar->FindVar("mat_fullbright"));
		printf("sv_pure = 0x%X\n", (DWORD)Interfaces.Cvar->FindVar("sv_pure"));
		printf("sv_consistency = 0x%X\n", (DWORD)Interfaces.Cvar->FindVar("sv_consistency"));
	}

	/*
	dh::gDeviceInternal = dh::FindDirexcXDevice();
	if (dh::gDeviceInternal)
	{
		gDirectXHook = new CVMTHookManager(dh::gDeviceInternal);
		oReset = gDirectXHook->SetupHook(16, Hooked_Reset);
		oEndScene = gDirectXHook->SetupHook(42, Hooked_EndScene);
		oDrawIndexedPrimitive = gDirectXHook->SetupHook(82, Hooked_DrawIndexedPrimitive);
		oCreateQuery = gDirectXHook->SetupHook(118, Hooked_CreateQuery);

		printf("Hook pD3DDevice 完毕。\n");
		printf("oReset = 0x%X\n", (DWORD)oReset);
		printf("oEndScene = 0x%X\n", (DWORD)oEndScene);
		printf("oDrawIndexedPrimitive = 0x%X\n", (DWORD)oDrawIndexedPrimitive);
		printf("oCreateQuery = 0x%X\n", (DWORD)oCreateQuery);
	}
	*/
	
	dh::StartDeviceHook([&](IDirect3D9* pD3D, IDirect3DDevice9* pDevice, DWORD* pVMT) -> void
	{
		dReset = DetourXS((void*)pVMT[16], Hooked_Reset);
		oReset = (FnReset)dReset.GetTrampoline();
		dEndScene = DetourXS((void*)pVMT[42], Hooked_EndScene);
		oEndScene = (FnEndScene)dEndScene.GetTrampoline();
		dDrawIndexedPrimitive = DetourXS((void*)pVMT[82], Hooked_DrawIndexedPrimitive);
		oDrawIndexedPrimitive = (FnDrawIndexedPrimitive)dDrawIndexedPrimitive.GetTrampoline();
		dCreateQuery = DetourXS((void*)pVMT[118], Hooked_CreateQuery);
		oCreateQuery = (FnCreateQuery)dCreateQuery.GetTrampoline();
		dPresent = DetourXS((void*)pVMT[17], Hooked_Present);
		oPresent = (FnPresent)dPresent.GetTrampoline();

		printf("Hook pD3DDevice 完毕。\n");
		printf("oReset = 0x%X\n", (DWORD)oReset);
		printf("oEndScene = 0x%X\n", (DWORD)oEndScene);
		printf("oDrawIndexedPrimitive = 0x%X\n", (DWORD)oDrawIndexedPrimitive);
		printf("oCreateQuery = 0x%X\n", (DWORD)oCreateQuery);
		printf("oPresent = 0x%X\n", (DWORD)oPresent);
	});

	DWORD client, engine, material, fmWait;
	client = Utils::GetModuleBase("client.dll");
	engine = Utils::GetModuleBase("engine.dll");
	material = Utils::GetModuleBase("materialsystem.dll");
	printf("client.dll = 0x%X\n", client);
	printf("engine.dll = 0x%X\n", engine);
	printf("materialsystem.dll = 0x%X\n", material);
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)pure, (void*)engine, NULL, NULL);
	
	fmWait = 45;
	bindAlias(fmWait);

	for (;;)
	{
		if (Interfaces.Engine->IsInGame() && !Interfaces.Engine->IsConsoleVisible())
		{
			if (GetAsyncKeyState(VK_SPACE) & 0x8000)
				bunnyHop();

			if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
				autoPistol();

			if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
				esp();

			if (GetAsyncKeyState(VK_XBUTTON2) & 0x8000)
				autoAim();
			else if (pCurrentAiming != nullptr)
				pCurrentAiming = nullptr;

			if (GetAsyncKeyState(VK_INSERT) & 0x01)
			{
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

				Sleep(1000);
			}

			if (GetAsyncKeyState(VK_HOME) & 0x01)
			{
				if(Utils::readMemory<int>(material + mat_fullbright) == 1)
					Utils::writeMemory(0, material + mat_fullbright);
				else
					Utils::writeMemory(1, material + mat_fullbright);

				Interfaces.Engine->ClientCmd("echo \"mat_fullbright set %d\"",
					Utils::readMemory<int>(material + mat_fullbright));

				Sleep(1000);
			}

			if (GetAsyncKeyState(VK_PRIOR) & 0x01)
			{
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

				Sleep(1000);
			}

			if (GetAsyncKeyState(VK_ADD) & 0x01)
			{
				Interfaces.Engine->ClientCmd("alias fastmelee_loop \"+attack; slot1; wait 1; -attack; slot2; wait %d; fastmelee_launcher\"", ++fmWait);
				Interfaces.Engine->ClientCmd("echo \"fastmelee wait set %d\"", fmWait);
				Sleep(1000);
			}

			if (GetAsyncKeyState(VK_SUBTRACT) & 0x01)
			{
				Interfaces.Engine->ClientCmd("alias fastmelee_loop \"+attack; slot1; wait 1; -attack; slot2; wait %d; fastmelee_launcher\"", --fmWait);
				Interfaces.Engine->ClientCmd("echo \"fastmelee wait set %d\"", fmWait);
				Sleep(1000);
			}
		}

		Sleep(1);
	}

}

void pure(void* engine)
{
	for (;;)
	{
		if (Utils::readMemory<int>((DWORD)engine + sv_pure) != 0 ||
			Utils::readMemory<int>((DWORD)engine + sv_consistency) != 0)
		{
			Utils::writeMemory(0, (DWORD)engine + sv_pure);
			Utils::writeMemory(0, (DWORD)engine + sv_consistency);

			Interfaces.Engine->ClientCmd("echo \"sv_pure and sv_consistency set %d\"",
				Utils::readMemory<int>((DWORD)engine + sv_pure));
		}

		Sleep(100);
	}
}

void __stdcall Hooked_CreateMove(int sequence_number, float input_sample_frametime, bool active)
{
	oCreateMove(sequence_number, input_sample_frametime, active);

	DWORD dwEBP;
	__asm mov dwEBP, ebp
	PBYTE pSendPacket = (PBYTE)(*(PDWORD)(dwEBP) - 0x21);

	CVerifiedUserCmd* verified = &(*(CVerifiedUserCmd**)((DWORD)Interfaces.Input + 0xE0))[sequence_number % 150];
	// CUserCmd* cmd = &(*(CUserCmd**)((DWORD_PTR)Interfaces.Input + 0xDC))[sequence_number % 150];
	CUserCmd* cmd = Interfaces.Input->GetUserCmd(sequence_number);
	CBaseEntity* client = GetLocalClient();
	
	if (!client || !cmd)
		return;

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

	verified->m_cmd = *cmd;
	verified->m_crc = cmd->GetChecksum();
}

bool __stdcall Hooked_CreateMoveShared(float flInputSampleTime, CUserCmd* cmd)
{
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
	static unsigned int MatSystemTopPanel = 0;
	static unsigned int FocusOverlayPanel = 0;

	if (MatSystemTopPanel == 0 || FocusOverlayPanel == 0)
	{
		const char* panelName = Interfaces.Panel->GetName(panel);
		if (panelName[0] == 'M' && panelName[3] == 'S' && panelName[9] == 'T')
			MatSystemTopPanel = panel;
		else if (panelName[0] == 'F' && panelName[5] == 'O')
			FocusOverlayPanel = panel;
	}

	if (FocusOverlayPanel > 0 && panel == FocusOverlayPanel)
	{

	}

	if (MatSystemTopPanel > 0 && panel == MatSystemTopPanel)
	{
		Interfaces.Surface->SetDrawColor(255, 255, 255, 255);
		Interfaces.Surface->DrawSetTextColor(255, 255, 128, 255);
		Interfaces.Surface->DrawFilledRect(100, 100, 200, 200);
	}

	// ((FnPaintTraverse)Interfaces.PanelHook->GetOriginalFunction(indexes::PaintTraverse))(ecx, panel, forcePaint, allowForce);
	oPaintTraverse(pPanel, panel, forcePaint, allowForce);
}

void __stdcall Hooked_FrameStageNotify(ClientFrameStage_t stage)
{
	QAngle aim, view;
	
	if (stage == FRAME_RENDER_START && Interfaces.Engine->IsInGame())
	{
		aim = GetLocalClient()->GetAimPunch();
		view = GetLocalClient()->GetViewPunch();
	}

	oFrameStageNotify(stage);

	if (aim && view)
	{
		GetLocalClient()->GetAimPunch() = aim;
		GetLocalClient()->GetViewPunch() = view;
	}
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
	return oReset(device, pp);
}

HRESULT WINAPI Hooked_EndScene(IDirect3DDevice9* device)
{
	return oEndScene(device);
}

HRESULT WINAPI Hooked_DrawIndexedPrimitive(IDirect3DDevice9* device, D3DPRIMITIVETYPE type, INT baseIndex,
	UINT minIndex, UINT numVertices, UINT startIndex, UINT primitiveCount)
{
	IDirect3DVertexBuffer9* stream = nullptr;
	UINT offsetByte, stride;
	device->GetStreamSource(0, &stream, &offsetByte, &stride);
	if (l4d2_special(stride, numVertices, primitiveCount))
	{
		static DWORD oldZEnable;
		device->GetRenderState(D3DRS_ZENABLE, &oldZEnable);
		device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
		device->SetRenderState(D3DRS_ZFUNC, D3DCMP_NEVER);
		oDrawIndexedPrimitive(device, type, baseIndex, minIndex, numVertices, startIndex, primitiveCount);
		device->SetRenderState(D3DRS_ZENABLE, oldZEnable);
		device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
		oDrawIndexedPrimitive(device, type, baseIndex, minIndex, numVertices, startIndex, primitiveCount);
	}
	
	return oDrawIndexedPrimitive(device, type, baseIndex, minIndex, numVertices, startIndex, primitiveCount);
}

HRESULT WINAPI Hooked_CreateQuery(IDirect3DDevice9* device, D3DQUERYTYPE type, IDirect3DQuery9** query)
{
	if (type == D3DQUERYTYPE_OCCLUSION)
		type = D3DQUERYTYPE_TIMESTAMP;
	
	return oCreateQuery(device, type, query);
}

HRESULT WINAPI Hooked_Present(IDirect3DDevice9* device, const RECT* source, const RECT* dest, HWND window, const RGNDATA* region)
{
	return oPresent(device, source, dest, window, region);
}

bool IsEnemyVisible(CBaseEntity* enemy)
{
	CBaseEntity* client = GetLocalClient();
	Vector end = enemy->GetAbsOrigin();

	trace_t trace;
	Ray_t ray;

	CTraceFilter filter;
	filter.pSkip1 = client;

	ray.Init(client->GetEyePosition(), end);
	Interfaces.Trace->TraceRay(ray, MASK_SHOT, &filter, &trace);
	if (trace.m_pEnt && trace.m_pEnt->GetTeam() != client->GetTeam() && trace.m_pEnt->GetHealth() > 0 &&
		!trace.m_pEnt->IsDormant() && trace.physicsBone <= 128 && trace.physicsBone > 0)
		return true;

	return false;
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

	if(!position.IsValid() || position.IsZero(1e-6f))
	{
		position = player->GetEyePosition();
		if (zombieClass == ZC_JOCKEY)
			position.z = player->GetAbsOrigin().z + 30.0f;
	}

	return position;
}

void bunnyHop()
{
	CBaseEntity* client = GetLocalClient();
	if (client && Interfaces.Engine->IsInGame() && !Interfaces.Engine->IsConsoleVisible() &&
		client->IsAlive())
	{
		/*
		if (client->GetFlags() & FL_ONGROUND)
			Interfaces.Engine->ClientCmd("+jump");
		else
			Interfaces.Engine->ClientCmd("-jump");
		*/
		
		static bool repeat = false;
		int flags = client->GetFlags();
		if (flags != FL_CLIENT && flags != (FL_DUCKING|FL_CLIENT) && flags != (FL_INWATER|FL_CLIENT) &&
			flags != (FL_DUCKING|FL_CLIENT|FL_INWATER))
		{
			Interfaces.Engine->ClientCmd("+jump");
			repeat = true;
		}
		else if (flags == FL_CLIENT || flags == (FL_DUCKING | FL_CLIENT) || flags == (FL_INWATER | FL_CLIENT) ||
			flags == (FL_DUCKING | FL_CLIENT | FL_INWATER))
		{
			Interfaces.Engine->ClientCmd("-jump");
			if (repeat)
			{
				Sleep(16);
				Interfaces.Engine->ClientCmd("+jump");
				Sleep(1);
				Interfaces.Engine->ClientCmd("-jump");
				repeat = false;
			}
		}
	}

	Sleep(1);
}

void autoPistol()
{
	CBaseEntity* client = GetLocalClient();
	if (client && Interfaces.Engine->IsInGame() && !Interfaces.Engine->IsConsoleVisible() &&
		client->IsAlive())
	{
		CBaseEntity* weapon = (CBaseEntity*)client->ActiveWeapon();
		if(weapon)
			weapon = Interfaces.ClientEntList->GetClientEntityFromHandle(weapon);
		
		float serverTime = client->GetTickBase() * Interfaces.Globals->interval_per_tick;
		if (weapon && weapon->GetNetProp<int>("m_iClip1", "DT_BaseCombatWeapon") > 0 &&
			weapon->GetNetProp<float>("m_flNextPrimaryAttack", "DT_BaseCombatWeapon") <= serverTime)
		{
			int weaponId = weapon->GetWeaponID();
			if (weaponId == Weapon_Pistol || weaponId == Weapon_ShotgunPump ||
				weaponId == Weapon_ShotAuto || weaponId == Weapon_SniperHunting ||
				weaponId == Weapon_ShotgunChrome || weaponId == Weapon_SniperMilitary ||
				weaponId == Weapon_ShotgunSpas || weaponId == Weapon_PistolMagnum ||
				weaponId == Weapon_SniperAWP || weaponId == Weapon_SniperScout)
			{
				Interfaces.Engine->ClientCmd("+attack");
				Sleep(10);
				Interfaces.Engine->ClientCmd("-attack");
			}
		}
	}

	Sleep(1);
}

void autoAim()
{
	CBaseEntity* client = GetLocalClient();
	if (client && Interfaces.Engine->IsInGame() && !Interfaces.Engine->IsConsoleVisible() &&
		client->IsAlive())
	{
		CBaseEntity* weapon = (CBaseEntity*)client->ActiveWeapon();
		if (weapon)
			weapon = Interfaces.ClientEntList->GetClientEntityFromHandle(weapon);

		if (weapon)
		{
			// 当前位置
			Vector myOrigin = client->GetEyePosition();

			// 检查是否需要选择新的敌人
			if (pCurrentAiming == nullptr || !pCurrentAiming->IsAlive() || pCurrentAiming->GetHealth() <= 0)
			{
				Vector myAngles;
				Interfaces.Engine->GetViewAngles(myAngles);

				int team = client->GetTeam();
				float distance = 32767.0f, dist, fov;
				
				// int max = Interfaces.ClientEntList->GetHighestEntityIndex();
				for (int i = 1; i <= 64; ++i)
				{
					CBaseEntity* target = Interfaces.ClientEntList->GetClientEntity(i);
					if (target == nullptr || target->GetTeam() == team || !target->IsAlive() ||
						target->GetHealth() <= 0 || target->IsDormant())
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
					dist = target->GetAbsOrigin().DistTo(myOrigin);
					fov = GetAnglesFieldOfView(myAngles, CalculateAim(myOrigin, target->GetEyePosition()));
					if (dist < distance && fov <= 25.f)
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
				Vector position = pCurrentAiming->GetEyePosition();

				int zombieClass = pCurrentAiming->GetNetProp<int>("m_zombieClass", "DT_TerrorPlayer");
				if (zombieClass == ZC_JOCKEY)
					position.z = pCurrentAiming->GetAbsOrigin().z + 30.0f;
				else if(zombieClass == ZC_HUNTER && (pCurrentAiming->GetFlags() & FL_DUCKING))
					position.z -= 12.0f;

				// Vector position = GetHeadPosition(pCurrentAiming);
				// Vector position = pCurrentAiming->GetHitboxPosition(0);
				/*
				if (position.x == 0.0f && position.y == 0.0f && position.z == 0.0f)
				{
					position = pCurrentAiming->GetEyePosition();
					
					if (zombieClass == ZC_JOCKEY)
						position.z = pCurrentAiming->GetAbsOrigin().z + 30.0f;
				}
				*/

				/*
				switch (zombieClass)
				{
				case ZC_SMOKER:
				case ZC_HUNTER:
				case ZC_TANK:
					position = pCurrentAiming->GetBonePosition(BONE_NECK);
					break;
				case ZC_BOOMER:
					position = pCurrentAiming->GetBonePosition(BONE_BOOMER_CHEST);
					break;
				case ZC_SPITTER:
				case ZC_JOCKEY:
					position = pCurrentAiming->GetBonePosition(BONE_JOCKEY_HEAD);
					break;
				case ZC_CHARGER:
					position = pCurrentAiming->GetBonePosition(BONE_CHARGER_HEAD);
					break;
				default:
					position = pCurrentAiming->GetEyePosition();
					if (zombieClass == ZC_JOCKEY)
						position.z = pCurrentAiming->GetAbsOrigin().z + 30.0f;
				}
				*/

				// VectorNormalize(position);
				Interfaces.Engine->SetViewAngles(CalculateAim(myOrigin, position));
			}
		}

		Sleep(1);
	}

	Sleep(1);
}

void esp()
{
	if (Interfaces.Engine->IsInGame())
	{
		for (int i = 1; i <= 64; ++i)
		{
			CBaseEntity* player = Interfaces.ClientEntList->GetClientEntity(i);
			if (player == nullptr || !player->IsAlive() || player->GetHealth() <= 0 || player->IsDormant())
				continue;

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

void bindAlias(int wait)
{
	Interfaces.Engine->ClientCmd("echo \"========= alias begin =========\"");
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
	Interfaces.Engine->ClientCmd("alias fastmelee_loop \"+attack; slot1; wait 1; -attack; slot2; wait %d; fastmelee_launcher\"", wait);
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
	Interfaces.Engine->ClientCmd("echo \"========= alias end =========\"");
}
