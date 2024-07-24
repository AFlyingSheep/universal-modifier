#include <windows.h>

#include <iostream>

#include "HollowKnight/knight.hpp"
#include "baseAddressFind.hpp"
#include "kernel.hpp"
#include "memoryOffset.h"

int main() {
  ModifierKernel<Knight> kernel;
  kernel.run(-1, 1000);
  // kernel.init_value(100, 100, 10000);
  // kernel.run_test();

  return 0;
}
