#include "get_process_name.h"
#include "look_for_process.h"
#include "memory_offsets.h"
#include <windows.h>
#include <psapi.h>
#include <iostream>
#include <tchar.h>
#include <processthreadsapi.h>

void GetProcessNameById(DWORD pId, int newHealthValueFromInput)
{
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pId);
    if (!hProcess)
        return;

    HMODULE hMod;
    DWORD cbNeeded;
    TCHAR procName[MAX_PATH] = _T("");

    if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))
    {
        if (GetModuleBaseName(hProcess, hMod, procName, MAX_PATH))
        {
            if (lookForMyProcess(procName))
            {
                MODULEINFO modInfo;
                GetModuleInformation(hProcess, hMod, &modInfo, sizeof(modInfo));
                uintptr_t base = (uintptr_t)modInfo.lpBaseOfDll;

                uintptr_t addr = base + GAME_POINTER_OFFSET;
                uintptr_t ptr1 = 0, ptr2 = 0;
                SIZE_T bytesRead;

                if (ReadProcessMemory(hProcess, (void *)addr, &ptr1, sizeof(ptr1), &bytesRead))
                {
                    if (ReadProcessMemory(hProcess, (void *)ptr1, &ptr2, sizeof(ptr2), &bytesRead))
                    {
                        uintptr_t healthAddr = ptr2 + PLAYER_HEALTH_OFFSET;
                        WriteProcessMemory(hProcess, (void *)healthAddr, &newHealthValueFromInput, sizeof(int), nullptr);
                    }
                }
            }
        }
    }

    CloseHandle(hProcess);
}
