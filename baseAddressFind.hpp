#include <Windows.h>
#include <tlhelp32.h>
#include <Psapi.h>
#include <iostream>

using namespace std;


DWORD GetProcessIdByName(const string& processName) {
    DWORD processId = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32);

        if (Process32First(hSnapshot, &pe32)) {
            do {
                if (_stricmp(pe32.szExeFile, processName.c_str()) == 0) {
                    processId = pe32.th32ProcessID;
                    break;
                }
            } while (Process32Next(hSnapshot, &pe32));
        }

        CloseHandle(hSnapshot);
    }

    return processId;
}


uintptr_t findBaseAddress(const char* processName, const char* targetDllName) {
    DWORD processId = GetProcessIdByName(processName); // Replace with the target process ID

    // Step 1: 获取进程句柄
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (hProcess == NULL) {
        cout << "Failed to open process. Error: " << GetLastError() << endl;
        return 0;
    }

    // Step 2: 枚举模块
    HMODULE hModules[1024];
    DWORD cbNeeded;
    if (!EnumProcessModules(hProcess, hModules, sizeof(hModules), &cbNeeded)) {
        cout << "Failed to enumerate process modules. Error: " << GetLastError() << endl;
        CloseHandle(hProcess);
        return 0;
    }

    // Step 3 & 4: 获取目标 DLL 的基址
    HMODULE hTargetModule = NULL;
    char szModuleName[MAX_PATH];
    for (DWORD i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
        if (GetModuleFileNameEx(hProcess, hModules[i], szModuleName, sizeof(szModuleName) / sizeof(char))) {
            char* moduleName = strrchr(szModuleName, '\\');
            if (moduleName != NULL) {
                moduleName++;
                if (_stricmp(moduleName, targetDllName) == 0) {
                    hTargetModule = hModules[i];
                    break;
                }
            }
        }
    }

    if (hTargetModule == NULL) {
        cout << "Target DLL not found in the process." << endl;
        CloseHandle(hProcess);
        return 1;
    }

    // Step 5: 获取基址
    uintptr_t baseAddress = (uintptr_t)hTargetModule;
    // cout << "Base address of " << targetDllName << " in process " << processId << ": 0x" << hex << baseAddress << endl;

    // Step 6: 关闭进程句柄
    CloseHandle(hProcess);

    return baseAddress;
}
