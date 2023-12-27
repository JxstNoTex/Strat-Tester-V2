#include "std_include.hpp"
#include "resource.h"
#include "inject.h"



#define OFF_ScrVarGlob REBASE(0x51A3500)
#define OFFSET(x)((INT64)GetModuleHandle(NULL) + (INT64)x)

static void get()
{

}

bool injector::injectT7()
{

    
    bool injectResponse = false;


    if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCSTR)&get, &hm) == 0)
    {
        int ret = GetLastError();
        printf("nope");
        // Return or however you want to handle an error.
    }
    else
    {

            //preload gscc
            Hres_GSCC = FindResource(hm, MAKEINTRESOURCE(GSCC), (LPCSTR)"BIN");
            HGlobal_GSCC = LoadResource(hm, Hres_GSCC);
            pointer = (void*)LockResource(HGlobal_GSCC);
            HSize_GSCC = SizeofResource(hm, Hres_GSCC);


            Hres_GSI = FindResource(hm, MAKEINTRESOURCE(GSI), (LPCSTR)"BIN");
            HGlobal_GSI = LoadResource(hm, Hres_GSI);
            pointer1 = (void*)LockResource(HGlobal_GSI);
            HSize_GSI = SizeofResource(hm, Hres_GSI);

            if (HSize_GSI <= 1)
            {
                printf("GSI wasnt allocated from resource");
            }
            if (HSize_GSCC <= 1)
            {
                printf("GSCC wasnt allocated from resource");
            }

            pID = injector::GetProcessIdByName("BlackOps3.exe");
            //GSCBuiltins::nlog("%d", pID);

            pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID);

            ReadProcessMemory(pHandle, (LPCVOID)OFFSET(0x9407AB0), &data_1, sizeof(data_1), 0);

            ReadProcessMemory(pHandle, (LPCVOID)(OFFSET(0x9407AB0) + 0x14), &data_2, sizeof(data_2), 0);

            UINT64 sptGlobal = data_1;
            int sptCount = data_2;

            for (int i = 0; i < sptCount; i++)
            {
                
                ReadProcessMemory(pHandle, (LPCVOID)(sptGlobal + (i * 0x18)), &t7spt, sizeof(t7spt), 0);
                char strBuff[256];
                UINT64 intbuff;

                ReadProcessMemory(pHandle, (LPCVOID)t7spt.llpName, &strBuff, sizeof(strBuff), 0);



                if (i == 34) //cause strBuff == "scripts/shared/duplicaterender_mgr.gsc" dosent work
                {
                    std::cout << "found it" << std::endl;
                    //GSCBuiltins::nlog("found it");
                    /*
                    std::cout << "---------------------------------------------------" << std::endl;
                    std::cout << "buffersize: " << t7spt.Buffersize << std::endl;
                    std::cout << "Pad: " << t7spt.Pad << std::endl;
                    std::cout << "llpName: " << t7spt.llpName << std::endl;
                    std::cout << "lpbuffer" << t7spt.lpBuffer << std::endl;
                    std::cout << "loaded Script number: " << i << std::endl;
                    std::cout << "String Name: " << strBuff << std::endl;
                    */
                    
                    
                    unsigned long long llpOriginalBuffer;
                    int OriginalSourceChecksum;

                    llpModifiedSPTStruct = i * sizeof(T7SPT) + sptGlobal;
                    llpOriginalBuffer = t7spt.lpBuffer;
                    ReadProcessMemory(pHandle, (LPCVOID)(llpOriginalBuffer + 0x8), &OriginalSourceChecksum, sizeof(OriginalSourceChecksum), 0);
                    /*std::cout << "---------------------------------------------------" << std::endl;
                    std::cout << "llpModifiedSPTStruct: " << llpModifiedSPTStruct << std::endl;
                    std::cout << "llpOriginalBuffer: " << llpOriginalBuffer << std::endl;
                    std::cout << "OriginalSourceChecksum: " << OriginalSourceChecksum << std::endl;*/

                    t7spt.lpBuffer = (long long)malloc(HSize_GSCC);
                    WriteProcessMemory(pHandle, (LPVOID)(t7spt.lpBuffer), (LPVOID)pointer, HSize_GSCC, 0);

                    injectedScript = t7spt;
                    InjectedBuffSize = t7spt.Buffersize;

                    WriteProcessMemory(pHandle, (LPVOID)(llpModifiedSPTStruct), (LPVOID)&t7spt, sizeof(t7spt), 0);


                    long long buf = t7spt.lpBuffer;
                    //printf("buffer to register = %x", buf);
                    RegisterDetours(pointer1, 1, t7spt.lpBuffer);
                    return injectResponse;
                }
                
            }
            
        return injectResponse;
    }
}


bool injector::FreeT7()
{

    free((void*)injectedScript.lpBuffer);
    WriteProcessMemory(pHandle, (LPVOID)(llpModifiedSPTStruct), (LPVOID)&t7spt, sizeof(t7spt), 0);
    return 0;
}

DWORD injector::GetProcessIdByName(const char* name)
{
    PROCESSENTRY32 pt;
    HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    pt.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(hsnap, &pt)) {
        do {
            if (!_stricmp(pt.szExeFile, name)) {
                CloseHandle(hsnap);
                return pt.th32ProcessID;
            }
        } while (Process32Next(hsnap, &pt));
    }
    CloseHandle(hsnap);
    return 0;
}