#include <windows.h>

#include <iostream>

#include "HollowKnight/knight.hpp"
#include "baseAddressFind.hpp"
#include "kernel.hpp"
#include "memoryOffset.h"

int main() {
  ModifierKernel<Knight> kernel;
  kernel.run(-1, 1000);

  return 0;
}
