#ifndef MEMORYOFFSET_H_
#define MEMORYOFFSET_H_

#include <Psapi.h>
#include <Windows.h>
#include <tlhelp32.h>

template <typename Derive, typename VType, size_t OFFSET_NUM>
struct OffsetBase {
  using ValueType = VType;

  static DWORD_PTR real_ptr;
  static int get_real_ptr(HANDLE& h_process, DWORD_PTR base) {
    real_ptr = base;
    for (int level = 0; level < OFFSET_NUM - 1; level++) {
      real_ptr += Derive::offset[level];
      // printf("Try to access %p\n", (void*)real_ptr);
      if (!ReadProcessMemory(h_process, (LPCVOID)real_ptr, &real_ptr,
                             sizeof(DWORD_PTR), NULL)) {
        printf("Open memory error.\n");
        return 1;
      }
      // printf("Get value %p\n", (void*)real_ptr);
    }
    real_ptr += Derive::offset[OFFSET_NUM - 1];
    return 0;
  }
  static ValueType get_default_value() { return default_value; }
  static void set_default_value(ValueType value) { default_value = value; }

  static ValueType default_value;
};

template <typename Derive, typename ValueType, size_t OFFSET_NUM>
DWORD_PTR OffsetBase<Derive, ValueType, OFFSET_NUM>::real_ptr = 0;
template <typename Derive, typename ValueType, size_t OFFSET_NUM>
ValueType OffsetBase<Derive, ValueType, OFFSET_NUM>::default_value = 100;

#endif  // MEMORYOFFSET_H_
