#ifndef KERNEL_HPP
#define KERNEL_HPP

#include <basetsd.h>
#include <memoryapi.h>
#include <synchapi.h>

#include <forward_list>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
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

    // Init default values by config file or fix values.
    init_value();
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

  // Implement of init_value
  template <typename TypeList, typename T>
  void set_init_value_impl(T&& value) {
    if constexpr (!IsEmpty<TypeList>::value) {
      using Type = Front<TypeList>;
      Type::set_default_value(value);
    }
  }

  // Implement of init_value
  template <typename TypeList, typename T, typename... Args>
  void set_init_value_impl(T&& value, Args&&... args) {
    if constexpr (!IsEmpty<TypeList>::value) {
      using Type = Front<TypeList>;
      Type::set_default_value(value);
      set_init_value_impl<PopFront<TypeList>>(std::forward<Args>(args)...);
    }
  }

  // Set the default value during compiling stage
  template <typename... Args>
  void set_init_value(Args&&... args) {
    set_init_value_impl<typename GameType::GameElement>(
        std::forward<Args>(args)...);
  }

  // Set the default value from config file
  // Default file name: config.json
  template <typename DefaultValueList, typename TypeList, std::size_t Index>
  void init_value_by_config_impl(DefaultValueList& value_list) {
    if constexpr (!IsEmpty<TypeList>::value) {
      using Type = Front<TypeList>;
      Type::set_default_value(value_list[Index]["value"]);
      init_value_by_config_impl<DefaultValueList, PopFront<TypeList>,
                                Index + 1>(value_list);
    }
  }

  // Init values by config file
  template <typename DefaultValueList>
  void init_value_by_config(DefaultValueList& value_list) {
    init_value_by_config_impl<DefaultValueList, typename GameType::GameElement,
                              0>(value_list);
  }

  void init_value(std::string config_file = "config.json") {
    std::ifstream stream(config_file);
    if (!stream) {
      std::cerr << "Can't open the config json: " << config_file << "."
                << std::endl;
      std::cerr << "Now use the default value.";
      return;
    }

    nlohmann::json config_json;
    stream >> config_json;

    init_value_by_config(config_json[GameType::configName]);
    std::cout << "Read the config file successfully." << std::endl;
  }

 private:
  DWORD_PTR base_pointer_;
  DWORD process_id_;
  HANDLE h_process_;
};

#endif  // KERNEL_HPP
