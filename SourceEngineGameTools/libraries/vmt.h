#pragma once
#include <cassert>

class CVMT
{
public:
	/*void* Hook(void* Instance, int Index, void* HookedFunc)
	{
	
		DWORD VirtualTable = *(DWORD*)Instance;

		DWORD VirtualFunction = VirtualTable + sizeof(DWORD) * Index;
		intptr_t OriginalFunction = *((DWORD*)VirtualFunction);

		DWORD OldProtect;
		VirtualProtect((LPVOID)VirtualFunction, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &OldProtect);
		*((DWORD*)VirtualFunction) = (DWORD)HookedFunc;
		VirtualProtect((LPVOID)VirtualFunction, sizeof(DWORD), OldProtect, &OldProtect);

		return (void*)OriginalFunction;
	
	}*/
	template<typename Function>
	Function getvfunc(PVOID Base, DWORD Index)
	{
		DWORD **VTablePointer = nullptr, *VTableFunctionBase = nullptr, dwAddress = NULL;
		
		__try
		{
			VTablePointer = (PDWORD*)Base;
			VTableFunctionBase = *VTablePointer;
			dwAddress = VTableFunctionBase[Index];
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			printv(VTablePointer);
			printv(VTableFunctionBase);
			printv(dwAddress);
			logerr("无法获取虚表函数地址，发生访问冲突。");
			return nullptr;
		}

		return (Function)(dwAddress);
	}

	inline void* GetFunction(void* Instance, int Index)
	{
		DWORD VirtualPointer = NULL;
		void* VirtualFunction = nullptr;

		__try
		{
			VirtualPointer = (*(DWORD*)Instance) + sizeof(DWORD) * Index;
			VirtualFunction = (void*)*((DWORD*)VirtualPointer);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			printv(VirtualPointer);
			printv(VirtualFunction);
			logerr("无法获取虚表函数地址，发生访问冲突。");
			return nullptr;
		}

		return VirtualFunction;
	}

};

CVMT VMT;
#define k_page_writeable (PAGE_READWRITE | PAGE_EXECUTE_READWRITE)
#define k_page_readable (k_page_writeable|PAGE_READONLY|PAGE_WRITECOPY|PAGE_EXECUTE_READ|PAGE_EXECUTE_WRITECOPY)
#define k_page_offlimits (PAGE_GUARD|PAGE_NOACCESS)
/*Credits to null for helping me with this.*/

class CVMTHookManager
{
public:
	CVMTHookManager(){}
	/*Deconstructor*/
	~CVMTHookManager(){
		this->HookTable(false);
	}
	CVMTHookManager(void* Interface)
	{
		this->Init(Interface);
	}
	/*Inits the function*/
	bool Init(void* Interface)
	{
		pOrgTable = *(void**)Interface;
		this->count = this->GetCount();
		pCopyTable = malloc(sizeof(void*)* count);
		memcpy(pCopyTable, pOrgTable, sizeof(void*) * count);
		pObject = (DWORD*)Interface;
		return true;
	}
	/*Hook/Unhook*/
	void HookTable(bool hooked)
	{
		if (hooked)
		{
			*pObject = (DWORD)pCopyTable;
		}
		else
		{
			*pObject = (DWORD)pOrgTable;
		}
	}

	/*Hooks function*/
	void* HookFunction(int Index, void* hkFunction)
	{
		if (Index < this->count && Index >= 0)
		{
			((DWORD*)pCopyTable)[Index] = (DWORD)hkFunction;
			return (void*)((DWORD*)pOrgTable)[Index];
		}
		return NULL;
	}

	void* GetOriginalFunction(int Index)
	{
		if (Index < this->count && Index >= 0)
			return (void*)((DWORD*)pOrgTable)[Index];
		
		return NULL;
	}

	template<typename Fn>
	Fn* SetupHook(int Index, Fn* hkFunction)
	{
		if (Index < this->count && Index >= 0)
		{
			((DWORD*)pCopyTable)[Index] = (DWORD)hkFunction;
			return (Fn*)((DWORD*)pOrgTable)[Index];
		}

		return nullptr;
	}

	template<typename Fn>
	Fn* GetFunction(int Index)
	{
		if (Index < this->count && Index >= 0)
			return (Fn*)((DWORD*)pOrgTable)[Index];

		return nullptr;
	}

private:
	/*Returns if you can read the pointer*/
	bool CanReadPointer(void* table)
	{
		MEMORY_BASIC_INFORMATION mbi;
		if (table == nullptr) return false;
		if (!VirtualQuery(table, &mbi, sizeof(mbi))) return false;
		if (mbi.Protect & k_page_offlimits) return false;
		return (mbi.Protect & k_page_readable);
	}
	/*Gets VMT count*/
	int GetCount()
	{
		int index = 0;
		void** table = ((void**)pOrgTable);
		__try
		{
			for (void* fn; (fn = table[index]) != nullptr; index++)
			{
				if (!this->CanReadPointer(fn)) break;
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			printf("VMT 0x%X access error %d\n", (DWORD)table, index--);
		}
		
		return index;
	}
	int count;
	void* pCopyTable;
	DWORD* pObject;
	void* pOrgTable;
};