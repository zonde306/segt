#pragma once
#include "../../Utils.h"
#include "../libraries/math.h"

static std::map<std::string, unsigned int> g_offsetList;
#define MAKE_NETPROP_GET(_fn,_nt,_np,_t)	inline _t& _fn()const{return this->GetNetProp<_t>(_np, _nt);}
#define MAKE_NETPROP_SET(_fn,_nt,_np,_t)	inline _t& _fn(const _t& val){return this->SetNetProp(_np, val, _nt);}

class CBaseEntity
{
public:
	float& GetFriction()
	{
		static int offset = netVars->GetOffset("DT_BasePlayer", "m_flFriction");
		return *(float*)(this + offset);
	}

	int& GetTeam()
	{
		static int offset = netVars->GetOffset("DT_BasePlayer", "m_iTeamNum");
		return *(int*)(this + offset);
	}

	Vector& GetVelocity()
	{
		static int offset = netVars->GetOffset("DT_BasePlayer", "m_vecVelocity[0]");
		return *(Vector*)(this + offset);
	}

	int& GetHealth()
	{
		static int offset = netVars->GetOffset("DT_BasePlayer", "m_iHealth");
		return *(int*)(this + offset);
	}

	int Index()
	{
		return *(int*)((DWORD)this + 64);
	}

	int& GetFlags()
	{
		static int offset = netVars->GetOffset("DT_BasePlayer", "m_fFlags");
		return *(int*)(this + offset);
	}

	QAngle& GetAimPunch()
	{
		static int offset = netVars->GetOffset("DT_BasePlayer", "m_aimPunchAngle");
		return *(QAngle*)(this + offset);
	}

	QAngle& GetViewPunch()
	{
		static int offset = netVars->GetOffset("DT_BasePlayer", "m_viewPunchAngle");
		return *(QAngle*)(this + offset);
	}

	int& GetTickBase()
	{
		static int offset = netVars->GetOffset("DT_BasePlayer", "m_nTickBase");
		return *(int*)(this + offset);
	}

	Vector GetEyePosition()
	{
		static int offset = netVars->GetOffset("DT_BasePlayer", "m_vecViewOffset[0]");
		Vector vecViewOffset = *(Vector*)(this + offset);
		return GetAbsOrigin() + vecViewOffset;
	}

	QAngle& GetEyeAngles() const
	{
		static int offset = netVars->GetOffset("DT_CSPlayer", "m_angEyeAngles[0]");
		return *(Vector*)(this + offset);
	}

	void* GetActiveWeapon()
	{
		static int offset = netVars->GetOffset("DT_BaseCombatCharacter", "m_hActiveWeapon");
		return *(void**)(this + offset);
	}

	MAKE_NETPROP_GET(GetClassname, "DT_BaseCombatCharacter", "m_iClassname", const char*);
	MAKE_NETPROP_GET(GetModelName, "DT_BaseCombatCharacter", "m_ModelName", const char*);
	MAKE_NETPROP_GET(GetName, "DT_BaseCombatCharacter", "m_iName", const char*);
	MAKE_NETPROP_GET(GetOrigin, "DT_BaseCombatCharacter", "m_vecOrigin", Vector);
	MAKE_NETPROP_GET(GetAngles, "DT_BaseCombatCharacter", "m_angRotation", Vector);
	MAKE_NETPROP_GET(GetEffects, "DT_BaseCombatCharacter", "m_fEffects", int);
	MAKE_NETPROP_GET(GetOwner, "DT_BaseCombatCharacter", "m_hOwnerEntity", void*);
	MAKE_NETPROP_GET(GetCollisionGroup, "DT_BaseCombatCharacter", "m_CollisionGroup", int);
	MAKE_NETPROP_GET(GetMins, "DT_BaseCombatCharacter", "m_vecMins", Vector);
	MAKE_NETPROP_GET(GetMaxs, "DT_BaseCombatCharacter", "m_vecMaxs", Vector);
	MAKE_NETPROP_GET(GetSolidType, "DT_BaseCombatCharacter", "m_nSolidType", int);
	MAKE_NETPROP_GET(GetSolidFlags, "DT_BaseCombatCharacter", "m_usSolidFlags", int);
	
	// 屏幕
	MAKE_NETPROP_GET(GetPunchAngles, "DT_BasePlayer", "m_vecPunchAngle", Vector);
	MAKE_NETPROP_GET(GetPunchVelocity, "DT_BasePlayer", "m_vecPunchAngleVel", Vector);
	MAKE_NETPROP_SET(SetPunchAngles, "DT_BasePlayer", "m_vecPunchAngle", Vector);
	MAKE_NETPROP_SET(SetPunchVelocity, "DT_BasePlayer", "m_vecPunchAngleVel", Vector);

	template<typename T>
	T& GetNetProp(const std::string& prop, const std::string& table = "DT_BaseEntity", size_t element = 0) const
	{
		if (g_offsetList.find(prop) == g_offsetList.end())
		{
			int offset = netVars->GetOffset(table.c_str(), prop.c_str());
			if (offset > -1)
			{
				Utils::log("%s %s offset is 0x%X", table.c_str(), prop.c_str(), offset);
				g_offsetList[prop] = (int)offset;
			}
			else
			{
				Utils::log("%s %s not found!", table.c_str(), prop.c_str());
			}
		}
		
		// VirtualProtect((T*)(this + m_offset[prop]), sizeof(T), PAGE_EXECUTE_READWRITE, NULL);
		return *(T*)(this + g_offsetList[prop] + element * sizeof(T));
	}

	template<typename T>
	T& SetNetProp(const std::string& prop, const T& value, const std::string& table = "DT_BaseEntity", size_t element = 0) const
	{
		if (g_offsetList.find(prop) == g_offsetList.end())
		{
			int offset = netVars->GetOffset(table.c_str(), prop.c_str());
			if (offset > -1)
			{
				Utils::log("%s %s offset is 0x%X", table.c_str(), prop.c_str(), offset);
				g_offsetList[prop] = (int)offset;
			}
			else
			{
				Utils::log("%s %s not found!", table.c_str(), prop.c_str());
			}
		}

		// 一般来说，变量都是可读可写的
		// VirtualProtect((T*)(this + m_offset[prop]), sizeof(T), PAGE_EXECUTE_READWRITE, NULL);
		return (*(T*)(this + g_offsetList[prop] + element * sizeof(T)) = value);
	}

	Vector& GetAbsOrigin()
	{
		typedef Vector& (__thiscall* OriginalFn)(void*);
		return ((OriginalFn)VMT.GetFunction(this, indexes::GetAbsOrigin))(this);
	}

	Vector& GetAbsAngles()
	{
		typedef Vector& (__thiscall* OriginalFn)(void*);
		return ((OriginalFn)VMT.GetFunction(this, indexes::GetAbsAngles))(this);
	}

	ClientClass* GetClientClass()
	{
		void* Networkable = (void*)(this + 0x8);
		typedef ClientClass* (__thiscall* OriginalFn)(void*);
		return ((OriginalFn)VMT.GetFunction(Networkable, indexes::GetClientClass))(Networkable);
	}
	
	bool IsDormant()
	{
		// return *reinterpret_cast<bool*>((DWORD)this + 0xE9);
		PVOID pNetworkable = static_cast<PVOID>(this + 0x8);
		typedef bool(__thiscall * OriginalFn)(PVOID);
		return ((OriginalFn)VMT.GetFunction(pNetworkable, indexes::IsDormant))(pNetworkable);
	}

	bool IsAlive()
	{
		static int offset = netVars->GetOffset("DT_BasePlayer", "m_lifeState");
		BYTE lifestate = *(BYTE*)((DWORD)this + offset);
		return (lifestate == 0);
	}

	int GetIndex()
	{
		void* networkable = (void*)(this + 0x8);
		typedef int(__thiscall* OriginalFn)(PVOID);
		return ((OriginalFn)VMT.GetFunction(this, indexes::GetIndex))(this);
	}

	bool SetupBones(VMatrix *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
	{
		void *pRenderable = (void*)(this + 0x4);
		typedef bool(__thiscall* OriginalFn)(PVOID, VMatrix*, int, int, float);
		return ((OriginalFn)VMT.GetFunction(pRenderable, indexes::SetupBones))(pRenderable, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
	}

	model_t* GetModel()
	{
		void *pRenderable = (void*)(this + 0x4);
		typedef model_t*(__thiscall* OriginalFn)(PVOID);
		return VMT.getvfunc<OriginalFn>(pRenderable, indexes::GetModel)(pRenderable);
	}

	int GetWeaponID()
	{
		typedef int(__thiscall *OriginalFn)(PVOID);
		return VMT.getvfunc<OriginalFn>(this, indexes::GetWeaponId)(this);
	}

	Vector GetHitboxPosition(int Hitbox)
	{
		VMatrix matrix[128];
		model_t* mod;
		studiohdr_t* hdr;
		mstudiohitboxset_t* set;
		mstudiobbox_t* hitbox;
		Vector MIN, MAX, MIDDLE;

		__try
		{
			if (!this->SetupBones(matrix, 128, 0x00000100, GetTickCount64()))
			{
				logerr("获取骨头位置失败");
				return Vector();
			}

			if ((mod = this->GetModel()) == nullptr)
			{
				logerr("获取模型失败");
				return Vector();
			}

			if ((hdr = Interfaces.ModelInfo->GetStudioModel(mod)) == nullptr)
			{
				logerr("获取模型信息失败");
				return Vector();
			}

			if ((set = hdr->pHitboxSet(0)) == nullptr)
			{
				logerr("获取 Hitbox 组失败");
				return Vector();
			}

			if ((hitbox = set->pHitbox(Hitbox)) == nullptr)
			{
				logerr("搜索 Hitbox 失败");
				return Vector();
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			logerr("发生了一未知的些错误。");
			return Vector();
		}
		
		VectorTransform(hitbox->bbmin, matrix[hitbox->bone], MIN);
		VectorTransform(hitbox->bbmax, matrix[hitbox->bone], MAX);

		MIDDLE = (MIN + MAX) * 0.5f;
		return MIDDLE;
	}

	Vector GetBonePosition(int bone)
	{
		VMatrix boneMatrix[128];
		__try
		{
			if (this->SetupBones(boneMatrix, 128, 0x00000100, GetTickCount64()))
				return Vector(boneMatrix[bone][0][3], boneMatrix[bone][1][3], boneMatrix[bone][2][3]);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			logerr("发生了一未知的些错误。");
			return Vector();
		}

		return Vector();
	}

#ifdef m_iCrosshairsId
	int GetCrosshairsId()
	{
		return *(int*)(this + m_iCrosshairsId);
	}
#endif // m_iCrosshairsId
};