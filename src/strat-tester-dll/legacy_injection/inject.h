#pragma once

#include "detours.h"
#include "offsets.h"
#include "resource.h"

#include "resource.h"
#include <TlHelp32.h>

struct T7SPT
{
	public:
		long long llpName;
		int Buffersize;
		int Pad;
		long long lpBuffer;

};


class injector
{
	public:
		HMODULE hm = NULL;
		bool injectT7();
		bool FreeT7();
		DWORD GetProcessIdByName(const char* name);

		HRSRC Hres_GSCC;
		HGLOBAL HGlobal_GSCC;
		void* pointer;
		INT64 HSize_GSCC;

		HRSRC Hres_GSI;
		HGLOBAL HGlobal_GSI;
		void* pointer1;
		INT64 HSize_GSI;
		HANDLE pHandle;
	private:
		DWORD pID;
		
		T7SPT t7spt;
		T7SPT injectedScript;
		int InjectedBuffSize;
		long long int data_1 = 0;
		int data_2 = 0;
		unsigned long long llpModifiedSPTStruct;
};


