#pragma once

class CGameEvent
{
public:

	const char* GetName()
	{
		typedef const char* (__thiscall* OriginalFn)(PVOID);
		return VMT.getvfunc<OriginalFn>(this, 1)(this);
	}

	int GetInt(const char *keyName, int defaultValue)
	{
		typedef int(__thiscall* OriginalFn)(PVOID, const char *, int);
		return VMT.getvfunc<OriginalFn>(this, 6)(this, keyName, defaultValue);
	}
	bool GetBool(const char *keyName, bool value)
	{
		typedef bool(__thiscall* OriginalFn)(PVOID, const char *, bool);
		return VMT.getvfunc<OriginalFn>(this, 5)(this, keyName, value);
	}
	char GetString(const char *keyName, const char *value)
	{
		typedef bool(__thiscall* OriginalFn)(PVOID, const char *, const char *);
		return VMT.getvfunc<OriginalFn>(this, 9)(this, keyName, value);
	}
	/*bool GetBool(const char *keyName, bool value)
	{
	typedef bool(__thiscall* OriginalFn)(PVOID, const char *, bool);
	return getvfunc<OriginalFn>(this, 5)(this, keyName, value);
	}*/
};

class IGameEventListener2
{
public:
	virtual	~IGameEventListener2(void) {};

	// FireEvent is called by EventManager if event just occured
	// KeyValue memory will be freed by manager if not needed anymore
	virtual void FireGameEvent(CGameEvent *event) = 0;
};

class IGameEventManager
{
public:
	bool AddListener(IGameEventListener2 *listener, const char *name, bool bServerSide)
	{
		typedef bool(__thiscall* OriginalFn)(PVOID, IGameEventListener2*, const char*, bool);
		return VMT.getvfunc<OriginalFn>(this, 3)(this, listener, name, bServerSide);
	}

	bool FireEventClientSide(CGameEvent *event)
	{
		typedef bool(__thiscall* OriginalFn)(PVOID, CGameEvent*);
		return VMT.getvfunc<OriginalFn>(this, 8)(this, event);
	}
};
