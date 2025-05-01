#include "list_processes.h"
#include "get_process_name.h"
#include <windows.h>
#include <psapi.h>
#include <iostream>

int ListProcesses(int newHealthValue) {
    DWORD processes[1024], cbNeeded, cbProcesses;

    if (!EnumProcesses(processes, sizeof(processes), &cbNeeded)) {
        std::cout << "[-] Unable to fetch processes\n";
        return 1;
    }

    cbProcesses = cbNeeded / sizeof(DWORD);
    std::cout << "[*] Number of processes: " << cbProcesses << "\n";

    for (int i = 0; i < cbProcesses; i++) {
        if (processes[i] != 0) {
            GetProcessNameById(processes[i], newHealthValue);
        }
    }

    return 0;
}
