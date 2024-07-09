#include <windows.h>
#include <iostream>
#include "baseAddressFind.hpp"

template<typename Derive, typename ValueType, std::size_t OFFSET_NUM>
struct OffsetBase {
    static DWORD_PTR real_ptr;
    static int get_real_ptr(HANDLE& h_process, DWORD_PTR base) {
        real_ptr = base;
        for (int level = 0; level < OFFSET_NUM - 1; level++) {
            real_ptr += Derive::offset[level];
            printf("Try to access %p\n", real_ptr);
            if (!ReadProcessMemory(h_process, (LPCVOID)real_ptr, &real_ptr, sizeof(DWORD_PTR), NULL)) {
                printf("Open memory error.\n");
                return 1;
            }
            printf("Get value %p\n", real_ptr);
        }
        real_ptr += Derive::offset[OFFSET_NUM - 1];
        return 0;
    }

    static ValueType default_value;
};

template<typename Derive, typename ValueType, std::size_t OFFSET_NUM>
DWORD_PTR OffsetBase<Derive, ValueType, OFFSET_NUM>::real_ptr = 0;
template<typename Derive, typename ValueType, std::size_t OFFSET_NUM>
ValueType OffsetBase<Derive, ValueType, OFFSET_NUM>::default_value = 100;

struct KnightHealth : public OffsetBase<KnightHealth, int, 6> {
    static constexpr int offset[6] = {
        0x19B8BE0, 0x10, 0x88, 0x28, 0x58, 0x190
    };
};

// TODO: Set default value
struct KnightPower : public OffsetBase<KnightPower, int, 6> {
    static constexpr int offset[6] = {
        0x19B8BE0, 0x10, 0x88, 0x28, 0x58, 0x1CC
    };
};

int main() {
    std::string exeName = "hollow_knight.exe";
    std::string dllName = "UnityPlayer.dll";

    DWORD_PTR pointer = findBaseAddress(exeName.c_str(), dllName.c_str());
    printf("Find data base: %p\n", pointer);

    DWORD process_id = GetProcessIdByName(exeName.c_str());
    printf("Find process id: %d\n", process_id);
    HANDLE h_process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);
    if (h_process == NULL) {
        printf("Open process error.\n");
        return 1;
    } else {
        printf("Open process success.\n");
    }

    DWORD_PTR actual_address;
    int life_value, power;
    if (!ReadProcessMemory(h_process, (LPCVOID)pointer, &actual_address, sizeof(DWORD_PTR), NULL)) {
        printf("Open memory error.\n");
        return 1;
    } else {
        printf("Open base address success: %p.\n", actual_address);
    }

    KnightHealth::get_real_ptr(h_process, pointer);
    KnightPower::get_real_ptr(h_process, pointer);

    // for (int level = 0; level < KnightHealth::offset_num - 1; level++) {
    //     pointer += Knight::offset[level];
    //     if (!ReadProcessMemory(h_process, (LPCVOID)pointer, &pointer, sizeof(DWORD_PTR), NULL)) {
    //         printf("Open memory error.\n");
    //         return 1;
    //     }
    //     printf("Get value %p\n", pointer);
    // }
    // pointer += Knight::offset[Knight::offset_num - 1];

    if (!ReadProcessMemory(h_process, (LPCVOID)KnightHealth::real_ptr, &life_value, sizeof(int), NULL)) {
        printf("Open memory error.\n");
        return 1;
    }
    if (!ReadProcessMemory(h_process, (LPCVOID)KnightPower::real_ptr, &power, sizeof(int), NULL)) {
        printf("Open memory error.\n");
        return 1;
    }
    printf("Get life value %d and power %d\n", life_value, power);

    while (true) {
        if (!WriteProcessMemory(h_process, (LPVOID)KnightPower::real_ptr, &KnightPower::default_value, sizeof(int), NULL)) {
            printf("Write memory error.\n");
            return 1;
        }
        if (!WriteProcessMemory(h_process, (LPVOID)KnightHealth::real_ptr, &KnightHealth::default_value, sizeof(int), NULL)) {
            printf("Write memory error.\n");
            return 1;
        }
        printf("Rewrite life success.\n");
        Sleep(2000);
    }


    return 0;
}