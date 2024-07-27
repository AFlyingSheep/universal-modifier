#include <windows.h>

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

#include "HollowKnight/knight.hpp"
#include "baseAddressFind.hpp"
#include "detail/format.hpp"
#include "kernel.hpp"
#include "memoryOffset.h"
#include "runtime.hpp"

int main() {
  Runtime<Knight> runtime;
  runtime.start();
  // runtime.run_test();
  return 0;
}
