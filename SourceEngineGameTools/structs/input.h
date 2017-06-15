#pragma once

class CBaseEntity;

class CInput
{
public:
	CUserCmd *GetUserCmd(int sequence_number)
	{
		typedef CUserCmd*(__thiscall* Fn)(void*, int, int);
		return ((Fn)VMT.GetFunction(this, indexes::GetUserCmd))(this, 0, sequence_number);
	}
};

class CGameMovement
{
public:
	void ProcessMovement(CBaseEntity *pPlayer, PVOID moveData)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, CBaseEntity *pPlayer, PVOID moveData);
		VMT.getvfunc<OriginalFn>(this, indexes::ProccessMovement)(this, pPlayer, moveData);
	}
};

class CMoveHelper
{
public:
	void SetHost(CBaseEntity *pPlayer)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, CBaseEntity *pPlayer);
		VMT.getvfunc<OriginalFn>(this, indexes::SetHost)(this, pPlayer);
	}
};

class CPrediction
{
public:
	void SetupMove(CBaseEntity *player, CUserCmd *ucmd, PVOID movehelper, PVOID moveData)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, CBaseEntity*, CUserCmd*, PVOID, PVOID);
		VMT.getvfunc<OriginalFn>(this, indexes::SetupMove)(this, player, ucmd, movehelper, moveData);
	}

	void FinishMove(CBaseEntity *player, CUserCmd *ucmd, PVOID moveData)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, CBaseEntity*, CUserCmd*, PVOID);
		VMT.getvfunc<OriginalFn>(this, indexes::FinishMove)(this, player, ucmd, moveData);
	}
};
