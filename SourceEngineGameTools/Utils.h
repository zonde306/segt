#pragma once
#include <TlHelp32.h>

#define INRANGE( x, a, b ) ( x >= a && x <= b )
#define getBits( x ) ( INRANGE( ( x & ( ~0x20 ) ), 'A', 'F' ) ? ( ( x & ( ~0x20 ) ) - 'A' + 0xa ) : ( INRANGE( x, '0', '9' ) ? x - '0' : 0 ) )
#define getByte( x ) ( getBits( x[ 0 ] ) << 4 | getBits( x[ 1 ] ) )
extern HINSTANCE hModuleCheats;

class Utils
{
public:

	// GBK 转 UTF-8
	static std::string g2u(const std::string& strGBK)
	{
		string strOutUTF8 = "";
		WCHAR * str1;
		int n = MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, NULL, 0);
		str1 = new WCHAR[n];
		MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, str1, n);
		n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);
		char * str2 = new char[n];
		WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, NULL, NULL);
		strOutUTF8 = str2;
		delete[]str1;
		str1 = NULL;
		delete[]str2;
		str2 = NULL;
		return strOutUTF8;
	}

	// UTF-8 转 GBK
	static std::string u2g(const std::string& strUTF8)
	{
		int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, NULL, 0);
		WCHAR * wszGBK = new WCHAR[len + 1];
		memset(wszGBK, 0, len * 2 + 2);
		MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, wszGBK, len);

		len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
		char *szGBK = new char[len + 1];
		memset(szGBK, 0, len + 1);
		WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
		//strUTF8 = szGBK;  
		std::string strTemp(szGBK);
		delete[]szGBK;
		delete[]wszGBK;
		return strTemp;
	}

	// UTF-8 转 GB2312
	static char* utg(const char* utf8)
	{
		int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
		wchar_t* wstr = new wchar_t[len + 1];
		memset(wstr, 0, len + 1);
		MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
		len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
		char* str = new char[len + 1];
		memset(str, 0, len + 1);
		WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
		if (wstr) delete[] wstr;
		return str;
	}

	// GB2312 转 UTF-8
	static char* gtu(const char* gb2312)
	{
		int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
		wchar_t* wstr = new wchar_t[len + 1];
		memset(wstr, 0, len + 1);
		MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);
		len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
		char* str = new char[len + 1];
		memset(str, 0, len + 1);
		WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
		if (wstr) delete[] wstr;
		return str;
	}

	// wchar 转 char
	static std::string w2c(const std::wstring& ws)
	{
		size_t convertedChars = 0;
		std::string curLocale = setlocale(LC_ALL, NULL); //curLocale="C"
		setlocale(LC_ALL, "chs");
		const wchar_t* wcs = ws.c_str();
		size_t dByteNum = sizeof(wchar_t)*ws.size() + 1;
		// cout << "ws.size():" << ws.size() << endl;            //5

		char* dest = new char[dByteNum];
		wcstombs_s(&convertedChars, dest, dByteNum, wcs, _TRUNCATE);
		// cout << "convertedChars:" << convertedChars << endl; //8
		std::string result = dest;
		delete[] dest;
		setlocale(LC_ALL, curLocale.c_str());
		return result;
	}

	// char 转 wchar
	static std::wstring c2w(const std::string& s)
	{
		size_t convertedChars = 0;
		std::string curLocale = setlocale(LC_ALL, NULL);   //curLocale="C"
		setlocale(LC_ALL, "chs");
		const char* source = s.c_str();
		size_t charNum = sizeof(char)*s.size() + 1;
		// cout << "s.size():" << s.size() << endl;   //7

		wchar_t* dest = new wchar_t[charNum];
		mbstowcs_s(&convertedChars, dest, charNum, source, _TRUNCATE);
		// cout << "s2ws_convertedChars:" << convertedChars << endl; //6
		std::wstring result = dest;
		delete[] dest;
		setlocale(LC_ALL, curLocale.c_str());
		return result;
	}

	// 搜索特征码，例如 8B 0D 74 8D 70 10 8B 01 8B 90 F8 01 00 00 FF D2 8B 04 85 4C 41 78 10 C3
	// 参数 dwAddress 为开始地址，参数 dwLength 需要搜索的范围，参数 szPattern 为特征码
	static DWORD FindPattern(DWORD dwAddress, DWORD dwLength, std::string szPattern)
	{
		const char *pat = szPattern.c_str();
		DWORD firstMatch = NULL;
		for (DWORD pCur = dwAddress; pCur < dwLength; pCur++)
		{
			if (!*pat)
				return firstMatch;
			if (*(PBYTE)pat == '\?' || *(BYTE *)pCur == getByte(pat))
			{
				if (!firstMatch)
					firstMatch = pCur;
				if (!pat[2])
					return firstMatch;
				if (*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?')
					pat += 3;
				else
					pat += 2;
			}
			else
			{
				pat = szPattern.c_str();
				firstMatch = 0;
			}
		}
		return NULL;
	}

	static HMODULE GetModuleHandleSafe(std::string pszModuleName)
	{
		HMODULE hmModuleHandle = NULL;

		do
		{
			hmModuleHandle = GetModuleHandleA(pszModuleName.c_str());
			Sleep(1);
		} while (hmModuleHandle == NULL);

		return hmModuleHandle;
	}

	// 获取 dll 地址
	static DWORD GetModuleBase(std::string ModuleName, DWORD ProcessID = 0)
	{
		if (ProcessID == 0)
			ProcessID = GetCurrentProcessId();
		
		MODULEENTRY32 lpModuleEntry = { 0 };
		HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, ProcessID);
		if (!hSnapShot)
			return NULL;

		lpModuleEntry.dwSize = sizeof(lpModuleEntry);
		BOOL __RunModule = Module32First(hSnapShot, &lpModuleEntry);
		while (__RunModule)
		{
			if (w2c(lpModuleEntry.szModule) == ModuleName)
			{
				CloseHandle(hSnapShot);
				return (DWORD)lpModuleEntry.modBaseAddr;
			}
			__RunModule = Module32Next(hSnapShot, &lpModuleEntry);
		}
		CloseHandle(hSnapShot);
		return NULL;
	}

	static DWORD GetModuleBase(std::string ModuleName, DWORD* ModuleSize, DWORD ProcessID = 0)
	{
		if (ProcessID == 0)
			ProcessID = GetCurrentProcessId();

		MODULEENTRY32 lpModuleEntry = { 0 };
		HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, ProcessID);
		if (!hSnapShot)
			return NULL;

		lpModuleEntry.dwSize = sizeof(lpModuleEntry);
		BOOL __RunModule = Module32First(hSnapShot, &lpModuleEntry);
		while (__RunModule)
		{
			if (w2c(lpModuleEntry.szModule) == ModuleName)
			{
				CloseHandle(hSnapShot);
				*ModuleSize = lpModuleEntry.dwSize;
				return (DWORD)lpModuleEntry.modBaseAddr;
			}
			__RunModule = Module32Next(hSnapShot, &lpModuleEntry);
		}
		CloseHandle(hSnapShot);
		return NULL;
	}

	// 根据进程名获取进程id
	static DWORD FindProccess(std::string proccessName)
	{
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
		PROCESSENTRY32 pe;
		pe.dwSize = sizeof(pe);

		if (!Process32First(snapshot, &pe))
			return 0;

		while (Process32Next(snapshot, &pe))
		{
			if (w2c(pe.szExeFile) == proccessName)
				return pe.th32ProcessID;
		}

		return 0;
	}

	template<typename T, size_t len>
	static inline int GetArrayLength(const T(&arr)[len])
	{
		return (int)len;
	}

	template<typename T, typename ...Arg>
	static T readMemory(Arg... offset)
	{
		DWORD offsetList[] = { (DWORD)offset... };
		DWORD currentAddress = 0, finalAddress = 0, oldProtect = 0;
		int len = GetArrayLength(offsetList);
		if (len <= 0)
		{
			printf("请提供至少一个地址！\n");
			return T();
		}

		__try
		{
			for (int i = 0; i < len; ++i)
			{
				/*
				if (i == len - 1 || offsetList[i] == NULL)
				{
					if (finalAddress != NULL)
						return *(T*)finalAddress;

					printf("找不到任何东西。\n");
					return T();
				}
				*/

				currentAddress += offsetList[i];
				if (VirtualProtect((void*)currentAddress, sizeof(finalAddress), PAGE_EXECUTE_READWRITE, &oldProtect) == FALSE)
				{
					printf("错误：修改地址 0x%X 的保护失败。\n", currentAddress);
					return T();
				}

				if (i < len - 1)
				{
					finalAddress = *(DWORD*)currentAddress;
					// memcpy_s(&finalAddress, sizeof(finalAddress), (void*)current, sizeof(current));
				}
				else
					finalAddress = currentAddress;

#ifdef DEBUG
				if (VirtualProtect((void*)currentAddress, sizeof(finalAddress), oldProtect, NULL) == FALSE)
					printf("错误：恢复地址 0x%X 的保护失败。\n", currentAddress);
#else
				VirtualProtect((void*)currentAddress, sizeof(finalAddress), oldProtect, &oldProtect);
#endif

				// 将当前地址设置为最后的地址
				currentAddress = finalAddress;
			}

			if (finalAddress != NULL)
				return *(T*)finalAddress;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			printf("错误：读取地址为 0x%X 的内容失败。\n", currentAddress);
		}

		return T();
	}

	template<typename T, typename ...Arg>
	static T writeMemory(T value, Arg... offset)
	{
		DWORD offsetList[] = { (DWORD)offset... };
		DWORD currentAddress = 0, finalAddress = 0, oldProtect = 0;
		int len = GetArrayLength(offsetList);
		if (len <= 0)
		{
			printf("请提供至少一个地址！\n");
			return T();
		}

		__try
		{
			for (int i = 0; i < len; ++i)
			{
				/*
				if (i == len - 1 || offsetList[i] == NULL)
				{
					if (finalAddress != NULL)
						return (*(T*)finalAddress = value);

					printf("找不到任何东西。\n");
					return T();
				}
				*/

				currentAddress += offsetList[i];
				if (VirtualProtect((void*)currentAddress, sizeof(finalAddress), PAGE_EXECUTE_READWRITE, &oldProtect) == FALSE)
				{
					printf("错误：修改地址 0x%X 的保护失败。\n", currentAddress);
					return T();
				}

				if (i < len - 1)
				{
					finalAddress = *(DWORD*)currentAddress;
					// memcpy_s(&finalAddress, sizeof(finalAddress), (void*)current, sizeof(current));
				}
				else
					finalAddress = currentAddress;

#ifdef DEBUG
				if (VirtualProtect((void*)currentAddress, sizeof(finalAddress), oldProtect, NULL) == FALSE)
					printf("错误：恢复地址 0x%X 的保护失败。\n", currentAddress);
#else
				VirtualProtect((void*)currentAddress, sizeof(finalAddress), oldProtect, &oldProtect);
#endif

				// 将当前地址设置为最后的地址
				currentAddress = finalAddress;
			}

			if (finalAddress != NULL)
				return (*(T*)finalAddress = value);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			printf("错误：读取地址为 0x%X 的内容失败。\n", currentAddress);
		}

		return T();
	}

	static std::vector<std::string> split(const std::string& s, const std::string& delim = " ")
	{
		std::vector<std::string> result;
		size_t last = 0;
		size_t index = s.find_first_of(delim, last);
		while (index != std::string::npos)
		{
			result.push_back(s.substr(last, index - last));
			last = index + 1;
			index = s.find_first_of(delim, last);
		}
		if (index - last > 0)
		{
			result.push_back(s.substr(last, index - last));
		}

		return result;
	}

	static std::string trim(const std::string& s, const std::string& delim = " \r\n\t")
	{
		if (s.empty())
			return s;

		std::string result = s;
		for (char c : delim)
		{
			result.erase(0, result.find_first_not_of(c));
			result.erase(result.find_last_not_of(c) + 1);
		}

		return result;
	}

	static void log(const char* text, ...)
	{
		static std::string path;
		if (path.empty())
		{
			char buffer[MAX_PATH];
			GetModuleFileNameA(hModuleCheats, buffer, MAX_PATH);
			std::string tmp = buffer;
			path = tmp.substr(0, tmp.rfind('\\'));
		}
		
		char buffer[1024];

		time_t t;
		time(&t);

		tm tmp;
		localtime_s(&tmp, &t);

		// 文件创建日期
		strftime(buffer, 1024, "\\segt_%Y%m%d.log", &tmp);

		std::fstream file(path + buffer, std::ios::out|std::ios::app|std::ios::ate);

		// 日志写入时间
		strftime(buffer, 1024, "[%H:%M:%S] ", &tmp);
		file << buffer;

		// 格式化字符串
		va_list ap;
		va_start(ap, text);
		vsprintf_s(buffer, text, ap);
		va_end(ap);

		// 输出
		file << buffer << "\r\n";

		// 完毕
		file.close();
	}
};

class CNetVars
{
public:
	CNetVars()
	{
		_tables.clear();

		ClientClass* clientClass = Interfaces.Client->GetAllClasses();
		if (!clientClass)
		{
			printf("ERROR: ClientClass was not found\n");
			return;
		}

		while (clientClass)
		{
			RecvTable *recvTable = clientClass->m_pRecvTable;

			_tables.push_back(recvTable);

			clientClass = clientClass->m_pNext;
		}
	}

	int GetOffset(const char* tableName, const char* propName)
	{
		int offset = GetProp(tableName, propName);
		if (!offset)
		{
			printf("ERROR: Failed to find offset for prop: %s from table: %s\n", propName, tableName);
			return 0;
		}


		return offset;
	}

private:
	int GetProp(const char* tableName, const char* propName, RecvProp **prop = 0)
	{
		RecvTable* recvTable = GetTable(tableName);
		if (!recvTable)
		{
			printf("ERROR: Failed to find table: %s\n", tableName);
			return 0;
		}
		int offset = GetProp(recvTable, propName, prop);
		if (!offset)
		{
			printf("ERROR: Failed to find offset for prop: %s from table: %s\n", propName, tableName);
			return 0;
		}

		return offset;
	}
	int GetProp(RecvTable *recvTable, const char *propName, RecvProp **prop = 0)
	{
		int extraOffset = 0;
		for (int i = 0; i < recvTable->m_nProps; ++i)
		{
			RecvProp* recvProp = &recvTable->m_pProps[i];

			RecvTable *child = recvProp->m_pDataTable;

			if (child && (child->m_nProps > 0))
			{
				int tmp = GetProp(child, propName, prop);
				if (tmp)
				{
					extraOffset += (recvProp->m_Offset + tmp);
				}
			}

			if (_stricmp(recvProp->m_pVarName, propName))
				continue;
			if (prop)
			{
				*prop = recvProp;
			}
			return (recvProp->m_Offset + extraOffset);
		}
		return extraOffset;
	}

	RecvTable *GetTable(const char *tableName)
	{
		if (_tables.empty())
		{
			printf("ERROR: Failed to find table: %s (_tables is empty)\n", tableName);
			return 0;
		}

		for each (RecvTable* table in _tables)
		{
			if (!table)
				continue;
			if (_stricmp(table->m_pNetTableName, tableName) == 0)
				return table;
		}
		return 0;
	}

	std::vector<RecvTable*> _tables;
};

CNetVars* netVars;