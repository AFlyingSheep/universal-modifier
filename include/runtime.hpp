#include <synchapi.h>

#include "detail/format.hpp"
#include "detail/modifier_status.hpp"
#include "detail/status_output.hpp"
#include "kernel.hpp"

template <typename GameType>
class Runtime {
 public:
  Runtime() = default;

  ~Runtime() {
    if (kernel_) {
      delete kernel_;
    }
  }
  bool retry() {
    // Retry
    if (--timeout_ > 0) {
      UM_WARN("Retry to launch the kernel in %d times...\n", timeout_);
      Sleep(retry_interval_);
      return true;
    } else {
      // Kernel launch failed, exit with -1
      UM_ERROR("Kernel run error. The program will exit.\n");
      return false;
    }
  }

  void start() {
    // Start to run the kernel
    do {
      // Reset the pointer of kernel
      if (kernel_ != nullptr) {
        delete kernel_;
      }
      kernel_ = new ModifierKernel<GameType>();
      // If init kernel failed, wait a few seconds and try again
      if (UniversalModifier::last_error != PROCESS_STATUS::SUCCESS) {
        // Output the info
        PRINT_PROCESS_STATUS(UniversalModifier::last_error);
        // Retry
        if (retry()) {
          continue;
        } else {
          exit(-1);
        }
      }

      // Kernel init over, start to run
      UM_INFO("Kernel init successful. Start to run!\n");
      auto status = kernel_->start();
      if (status == MODIFIER_STATUS::SUCCESS) {
        UM_INFO("Kernel run over. The runtime will exit normally.\n");
        break;
      }
      // Kernel run error, check the status and try to rerun the kernel
      PRINT_MODIFIER_STATUS(status);

      // Retry
      if (retry()) {
        continue;
      } else {
        exit(-1);
      }

    } while (true);
  }
  void run_test() { kernel_->run_test(); }

 private:
  ModifierKernel<GameType>* kernel_;
  int timeout_ = 20;
  int retry_interval_ = 2000;
};
