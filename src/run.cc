#include <windows.h>

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

#include "HollowKnight/knight.hpp"
#include "baseAddressFind.hpp"
#include "kernel.hpp"
#include "memoryOffset.h"

int main() {
  ModifierKernel<Knight> kernel;
  kernel.run(-1, 1000);
  // kernel.run_test();

  return 0;
}
