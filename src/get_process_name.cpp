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
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, pId);

    if (hProcess != NULL)
    {
        HMODULE hMod;
        DWORD cbNeeded;
        TCHAR procName[MAX_PATH] = _T("<Unknown>");

        if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))
        {
            if (GetModuleBaseName(hProcess, hMod, procName, sizeof(procName) / sizeof(TCHAR)))
            {
                if (lookForMyProcess(procName))
                {
                    MODULEINFO modInfo = {0};

                    if (GetModuleInformation(hProcess, hMod, &modInfo, sizeof(modInfo)))
                    {
                        uintptr_t baseAddress = reinterpret_cast<uintptr_t>(modInfo.lpBaseOfDll);
                        uintptr_t pointerAddress = baseAddress + GAME_POINTER_OFFSET;
                        DWORD64 yellPointer = baseAddress + YELL_FUNCTION_OFFSET;
                        DWORD64 speakPointer = baseAddress + SPEAK_FUNCTION_OFFSET;

                        uintptr_t pointer1 = 0;
                        SIZE_T bytesRead = 0;

                        // Step 1: Read pointer1 from base + GAME_POINTER_OFFSET
                        if (ReadProcessMemory(hProcess, (LPCVOID)pointerAddress, &pointer1, sizeof(pointer1), &bytesRead))
                        {
                            std::cout << "[+] Pointer 1: 0x" << std::hex << pointer1 << std::endl;

                            uintptr_t pointer2 = 0;

                            // Step 2: Read pointer2 from pointer1
                            if (ReadProcessMemory(hProcess, (LPCVOID)pointer1, &pointer2, sizeof(pointer2), &bytesRead))
                            {
                                std::cout << "[+] Pointer 2 (Player Object): 0x" << std::hex << pointer2 << std::endl;

                                int playerHealth = 0;
                                uintptr_t healthAddr = pointer2 + PLAYER_HEALTH_OFFSET;

                                // Step 3: Read player health
                                if (ReadProcessMemory(hProcess, (LPCVOID)healthAddr, &playerHealth, sizeof(playerHealth), &bytesRead))
                                {
                                    std::cout << "[+] Player Health: " << std::dec << playerHealth << std::endl;

                                    int newHealthValue = newHealthValueFromInput;
                                    SIZE_T bytesWritten = 0;

                                    if (WriteProcessMemory(hProcess, (LPVOID)healthAddr, &newHealthValue, sizeof(newHealthValue), &bytesWritten))
                                    {
                                        std::cout << "[+] Managed to overwrite Player Health, new value is: " << std::dec << newHealthValue << std::endl;



                                        HANDLE hRemoteThread = CreateRemoteThread(
                                            hProcess,
                                            NULL,
                                            0,
                                            (LPTHREAD_START_ROUTINE)yellPointer, // Function pointer in remote process
                                            NULL,                                // No parameters
                                            0,
                                            NULL);
                                        
                                        if (hRemoteThread == NULL)
                                        {
                                            std::cerr << "[-] Failed to create remote thread. Error: " << GetLastError() << std::endl;
                                        }
                                        else
                                        {
                                            std::cout << "[+] Remote thread created to call yell()!" << std::endl;
                                            WaitForSingleObject(hRemoteThread, INFINITE);
                                            CloseHandle(hRemoteThread);
                                        }




                                    }
                                    else
                                    {
                                        std::cerr << "[-] Failed to overwrite Player Health. Error: " << GetLastError() << std::endl;
                                    }
                                }
                                else
                                {
                                    std::cerr << "[-] Failed to read player health. Error: " << GetLastError() << std::endl;
                                }
                            }
                            else
                            {
                                std::cerr << "[-] Failed to read pointer2. Error: " << GetLastError() << std::endl;
                            }
                        }
                        else
                        {
                            std::cerr << "[-] Failed to read pointer1. Error: " << GetLastError() << std::endl;
                        }
                    }
                    else
                    {
                        std::cerr << "[-] Failed to get module information. Error: " << GetLastError() << std::endl;
                    }
                }
            }
        }
    }
    CloseHandle(hProcess);
}
