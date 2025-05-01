#include "process_util.h"
#include <windows.h>
#include <iostream>

int main() {
    const wchar_t* pipeName = L"\\\\.\\pipe\\TestPipe";

    HANDLE hPipe = CreateNamedPipeW(
        pipeName,
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        1, 1024, 1024, 0, NULL);

    if (hPipe == INVALID_HANDLE_VALUE) {
        std::wcerr << L"Failed to create pipe. Error: " << GetLastError() << std::endl;
        return 1;
    }

    std::wcout << L"Waiting for client to connect...\n";
    if (ConnectNamedPipe(hPipe, NULL) || GetLastError() == ERROR_PIPE_CONNECTED) {
        char buffer[128];
        DWORD bytesRead;

        while (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
            buffer[bytesRead] = '\0';
            try {
                int newVal = std::stoi(std::string(buffer));
                ListProcesses(newVal);
            } catch (...) {
                std::cerr << "Invalid input\n";
            }

            const char* reply = "OK\n";
            DWORD bytesWritten;
            WriteFile(hPipe, reply, strlen(reply), &bytesWritten, NULL);
        }
    }

    CloseHandle(hPipe);
    return 0;
}
