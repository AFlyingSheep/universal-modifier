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
#include "detail/format.hpp"
#include "detail/modifier_status.hpp"
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
  MODIFIER_STATUS write_buffer() {
    if constexpr (!IsEmpty<TypeList>::value) {
      using Type = Front<TypeList>;
      if (Type::get_enable()) {
        if (!WriteProcessMemory(h_process_, (LPVOID)Type::real_ptr,
                                &Type::default_value,
                                sizeof(typename Type::ValueType), NULL)) {
          UM_ERROR("Write memory error in address: %p.\n",
                   (void*)Type::real_ptr);
          return MODIFIER_STATUS::WRITE_ERROR;
        }
      }
      return write_buffer<PopFront<TypeList>>();
    } else {
      return MODIFIER_STATUS::SUCCESS;
    }
  }

  template <typename TypeList>
  MODIFIER_STATUS cout_buff() {
    if constexpr (!IsEmpty<TypeList>::value) {
      using Type = Front<TypeList>;
      if (Type::get_enable()) {
        std::cout << Type::default_value << std::endl;
      }
      return cout_buff<PopFront<TypeList>>();
    } else {
      return MODIFIER_STATUS::SUCCESS;
    }
  }

  MODIFIER_STATUS run(int time, int interval) {
    bool inf = (time < 0);
    int global_round = 0;
    while (inf || time > 0) {
      global_round++;
      if (write_buffer<typename GameType::GameElement>() ==
          MODIFIER_STATUS::SUCCESS) {
        UM_INFO("Rewrite success, interval: %d ms, remaining round: %d.\n",
                interval, time);

        if (time > 0) time--;
        if (global_round % reload_time == 0) {
          init_pointer<typename GameType::GameElement>();
          UM_INFO("Reload the pointer over.\n");
        }

        Sleep(interval);
      } else {
        return MODIFIER_STATUS::WRITE_ERROR;
      }
    }
    return MODIFIER_STATUS::SUCCESS;
  }

  MODIFIER_STATUS start() { return run(repeat_time_, interval_); }

  MODIFIER_STATUS run_test() {
    return cout_buff<typename GameType::GameElement>();
  }

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
      // Set enable to false to disable the function
      Type::set_enable(value_list[Index]["enable"]);
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

  // Init global configuration by config file
  template <typename Typelist>
  void init_global_value(Typelist& value_list) {
    repeat_time_ = value_list["repeating"];
    interval_ = value_list["time_interval"];
  }

  void init_value(std::string config_file = "config.json") {
    std::ifstream stream(config_file);
    if (!stream) {
      UM_WARN("Can't open the config json: %s. Now use the default value.",
              config_file.c_str());
      return;
    }

    nlohmann::json config_json;
    stream >> config_json;

    init_value_by_config(config_json[GameType::configName]);
    init_global_value(config_json["global_config"]);
    UM_INFO("Read the config file and set value successfully.\n");
  }

 private:
  DWORD_PTR base_pointer_;
  DWORD process_id_;
  HANDLE h_process_;

  // Global configuration
  int repeat_time_;
  int interval_;

  // Reinit pointer time
  const int reload_time = 10;
};

#endif  // KERNEL_HPP
