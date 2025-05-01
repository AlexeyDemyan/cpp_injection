#pragma once
#include <windows.h>

BOOL lookForMyProcess(TCHAR* processName);
void GetProcessNameById(DWORD pId, int newHealthValue);
int ListProcesses(int newHealthValue);
