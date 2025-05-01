#include "look_for_process.h"
#include <tchar.h>
#include <iostream>

BOOL lookForMyProcess(TCHAR* processName) {
    TCHAR toFind[] = _T("game-run.exe");
    if (_tcscmp(processName, toFind) == 0) {
        std::wcout << L"[+] Found match: " << processName << std::endl;
        return TRUE;
    }
    return FALSE;
}
