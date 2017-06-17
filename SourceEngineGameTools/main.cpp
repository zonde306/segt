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
		InitGPUD3DHook();
		TryInitD3D(FindWindowA(NULL, "Left 4 Dead 2"));
	}
	return true;
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

static ConVar *sv_cheats, *r_drawothermodels, *cl_drawshadowtexture, *mat_fullbright;
static CBaseEntity* pCurrentAiming;

void bunnyHop();
void autoPistol();
void autoAim();
void esp();

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
		sv_cheats = Interfaces.Cvar->FindVar(XorStr("sv_cheats"));
		printf("sv_cheats = 0x%X\n", (DWORD)sv_cheats);

		r_drawothermodels = Interfaces.Cvar->FindVar(XorStr("r_drawothermodels"));
		printf("r_drawothermodels = 0x%X\n", (DWORD)r_drawothermodels);

		cl_drawshadowtexture = Interfaces.Cvar->FindVar(XorStr("cl_drawshadowtexture"));
		printf("cl_drawshadowtexture = 0x%X\n", (DWORD)cl_drawshadowtexture);

		mat_fullbright = Interfaces.Cvar->FindVar(XorStr("mat_fullbright"));
		printf("mat_fullbright = 0x%X\n", (DWORD)mat_fullbright);

	}

	for (;;)
	{
		if (Interfaces.Engine->IsInGame() && !Interfaces.Engine->IsConsoleVisible())
		{
			if (GetAsyncKeyState(VK_SPACE) & 0x8000)
				bunnyHop();

			if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
				autoPistol();

			if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
				esp();

			if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
				autoAim();
			else if (pCurrentAiming != nullptr)
				pCurrentAiming = nullptr;
		}

		Sleep(1);
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
		if (client->GetFlags() & FL_ONGROUND)
			Interfaces.Engine->ClientCmd("+jump");
		else
			Interfaces.Engine->ClientCmd("-jump");

		Sleep(10);
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
		
		if (weapon)
		{
			Interfaces.Engine->ClientCmd("+attack");
			Sleep(10);
			Interfaces.Engine->ClientCmd("-attack");
			Sleep(9);
		}

		Sleep(1);
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
				if (pCurrentAiming->GetNetProp<int>("m_zombieClass", "DT_TerrorPlayer") == ZC_JOCKEY)
					position.z = pCurrentAiming->GetAbsOrigin().z + 30.0f;;
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
			player->SetNetProp("m_nGlowRange", 65535, "DT_TerrorPlayer");
			player->SetNetProp("m_glowColorOverride", 65535, "DT_TerrorPlayer");
			// player->SetNetProp("m_bSurvivorGlowEnabled", true, "DT_TerrorPlayer");
		}
		
		Sleep(1);
	}

	Sleep(1);
}
