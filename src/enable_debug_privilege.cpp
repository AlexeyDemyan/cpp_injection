#include "enable_debug_privilege.h"
#include <windows.h>
#include <iostream>

BOOL EnableDebugPrivilege()
{
    HANDLE hToken;
    TOKEN_PRIVILEGES tp;
    LUID luid;

    // Open the current process token with TOKEN_ADJUST_PRIVILEGES and TOKEN_QUERY access
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
    {
        std::cerr << "OpenProcessToken failed. Error: " << GetLastError() << std::endl;
        return FALSE;
    }

    // Lookup the LUID for SE_DEBUG_NAME privilege
    if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid))
    {
        std::cerr << "LookupPrivilegeValue failed. Error: " << GetLastError() << std::endl;
        CloseHandle(hToken);
        return FALSE;
    }

    // Set up the TOKEN_PRIVILEGES structure
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    // Adjust the privileges
    if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL))
    {
        std::cerr << "AdjustTokenPrivileges failed. Error: " << GetLastError() << std::endl;
        CloseHandle(hToken);
        return FALSE;
    }

    // Check if the privilege is successfully enabled
    if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
    {
        std::cerr << "SE_DEBUG_NAME privilege is not assigned." << std::endl;
        CloseHandle(hToken);
        return FALSE;
    }

    // Close the token handle
    CloseHandle(hToken);
    return TRUE;
}