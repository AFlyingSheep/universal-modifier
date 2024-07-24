#ifndef KERNEL_HPP
#define KERNEL_HPP

#include <basetsd.h>
#include <memoryapi.h>
#include <synchapi.h>

#include <forward_list>
#include <iostream>
#include <utility>

#include "baseAddressFind.hpp"
#include "detail/typelist.hpp"

template <typename GameType>
class ModifierKernel {
 public:
  ModifierKernel() {
    // Get base address of dll or exe.
    base_pointer_ =
        findBaseAddress(GameType::exeName.c_str(), GameType::dllName.c_str());
    process_id_ = GetProcessIdByName(GameType::exeName.c_str());
    // Get handle of the process
    h_process_ = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id_);

    // Init all element's pointer
    init_pointer<typename GameType::GameElement>();
  }

  template <typename TypeList>
  int write_buffer() {
    if constexpr (!IsEmpty<TypeList>::value) {
      using Type = Front<TypeList>;
      if (!WriteProcessMemory(h_process_, (LPVOID)Type::real_ptr,
                              &Type::default_value,
                              sizeof(typename Type::ValueType), NULL)) {
        printf("Write memory error in address: %p.\n", (void*)Type::real_ptr);
        return 1;
      }
      return write_buffer<PopFront<TypeList>>();
    } else {
      return 0;
    }
  }

  template <typename TypeList>
  int cout_buff() {
    if constexpr (!IsEmpty<TypeList>::value) {
      using Type = Front<TypeList>;
      std::cout << Type::default_value << std::endl;
      return cout_buff<PopFront<TypeList>>();
    } else {
      return 0;
    }
  }

  void run(int time = -1, int interval = 1000) {
    bool inf = (time < 0);
    while (inf || time > 0) {
      if (!write_buffer<typename GameType::GameElement>()) {
        printf("Rewrite success, interval: %d ms, remaining round: %d.\n",
               interval, time);
        if (time > 0) {
          time--;
        }
        Sleep(interval);
      } else {
        printf("Rewrite error!");
        return;
      }
    }
  }

  void run_test() { cout_buff<typename GameType::GameElement>(); }

  template <typename TypeList>
  void init_pointer() {
    if constexpr (!IsEmpty<TypeList>::value) {
      using Type = Front<TypeList>;
      Type::get_real_ptr(h_process_, base_pointer_);
      init_pointer<PopFront<TypeList>>();
    }
  }

  template <typename TypeList, typename T>
  void init_value_impl(T&& value) {
    if constexpr (!IsEmpty<TypeList>::value) {
      using Type = Front<TypeList>;
      Type::set_default_value(value);
    }
  }

  template <typename TypeList, typename T, typename... Args>
  void init_value_impl(T&& value, Args&&... args) {
    if constexpr (!IsEmpty<TypeList>::value) {
      using Type = Front<TypeList>;
      Type::set_default_value(value);
      init_value_impl<PopFront<TypeList>>(std::forward<Args>(args)...);
    }
  }

  template <typename... Args>
  void init_value(Args&&... args) {
    init_value_impl<typename GameType::GameElement>(
        std::forward<Args>(args)...);
  }

 private:
  DWORD_PTR base_pointer_;
  DWORD process_id_;
  HANDLE h_process_;
};

#endif  // KERNEL_HPP
