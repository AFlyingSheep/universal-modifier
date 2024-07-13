#ifndef BASE_ADDRESS_FIND_HPP
#define BASE_ADDRESS_FIND_HPP

#include <Psapi.h>
#include <Windows.h>
#include <tlhelp32.h>

#include <iostream>

DWORD GetProcessIdByName(const std::string& processName) {
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
  DWORD processId =
      GetProcessIdByName(processName);  // Replace with the target process ID

  // Step 1: Get handler of process
  HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                FALSE, processId);
  if (hProcess == NULL) {
    std::cout << "Failed to open process. Error: " << GetLastError()
              << std::endl;
    return 0;
  }

  // Step 2: Enum the modules
  HMODULE hModules[1024];
  DWORD cbNeeded;
  if (!EnumProcessModules(hProcess, hModules, sizeof(hModules), &cbNeeded)) {
    std::cout << "Failed to enumerate process modules. Error: "
              << GetLastError() << std::endl;
    CloseHandle(hProcess);
    return 0;
  }

  // Step 3 & 4: Get the basic address of DLL
  HMODULE hTargetModule = NULL;
  char szModuleName[MAX_PATH];
  for (DWORD i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
    if (GetModuleFileNameEx(hProcess, hModules[i], szModuleName,
                            sizeof(szModuleName) / sizeof(char))) {
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
    std::cout << "Target DLL not found in the process." << std::endl;
    CloseHandle(hProcess);
    return 1;
  }

  // Step 5: Get the basic address
  uintptr_t baseAddress = (uintptr_t)hTargetModule;
  // cout << "Base address of " << targetDllName << " in process " << processId
  // << ": 0x" << hex << baseAddress << endl;

  // Step 6: Close the handler
  CloseHandle(hProcess);

  return baseAddress;
}

#endif  // BASE_ADDRESS_FIND_HPP
