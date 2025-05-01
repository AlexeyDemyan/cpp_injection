#include "process_util.h"
#include "memory_offsets.h"
#include <psapi.h>
#include <tchar.h>
#include <iostream>

BOOL lookForMyProcess(TCHAR* processName) {
    TCHAR toFind[] = _T("game-run.exe");
    return (_tcscmp(processName, toFind) == 0);
}

void GetProcessNameById(DWORD pId, int newHealthValueFromInput) {
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pId);
    if (!hProcess) return;

    HMODULE hMod;
    DWORD cbNeeded;
    TCHAR procName[MAX_PATH] = _T("");

    if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
        if (GetModuleBaseName(hProcess, hMod, procName, MAX_PATH)) {
            if (lookForMyProcess(procName)) {
                MODULEINFO modInfo;
                GetModuleInformation(hProcess, hMod, &modInfo, sizeof(modInfo));
                uintptr_t base = (uintptr_t)modInfo.lpBaseOfDll;

                uintptr_t addr = base + GAME_POINTER_OFFSET;
                uintptr_t ptr1 = 0, ptr2 = 0;
                SIZE_T bytesRead;

                if (ReadProcessMemory(hProcess, (void*)addr, &ptr1, sizeof(ptr1), &bytesRead)) {
                    if (ReadProcessMemory(hProcess, (void*)ptr1, &ptr2, sizeof(ptr2), &bytesRead)) {
                        uintptr_t healthAddr = ptr2 + PLAYER_HEALTH_OFFSET;
                        WriteProcessMemory(hProcess, (void*)healthAddr, &newHealthValueFromInput, sizeof(int), nullptr);
                    }
                }
            }
        }
    }

    CloseHandle(hProcess);
}

int ListProcesses(int newHealthValue) {
    DWORD processes[1024], cbNeeded;
    if (!EnumProcesses(processes, sizeof(processes), &cbNeeded)) return 1;

    int count = cbNeeded / sizeof(DWORD);
    for (int i = 0; i < count; i++) {
        if (processes[i]) {
            GetProcessNameById(processes[i], newHealthValue);
        }
    }
    return 0;
}
