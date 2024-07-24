#ifndef KNIGHT_HPP
#define KNIGHT_HPP

#include <string>

#include "detail/typelist.hpp"
#include "memoryOffset.h"

struct KnightHealth : public OffsetBase<KnightHealth, int, 6> {
  static constexpr int offset[6] = {0x19B8BE0, 0x10, 0x88, 0x28, 0x58, 0x190};
};

// TODO: Set default value
struct KnightPower : public OffsetBase<KnightPower, int, 6> {
  static constexpr int offset[6] = {0x19B8BE0, 0x10, 0x88, 0x28, 0x58, 0x1CC};
};

struct KnightGold : public OffsetBase<KnightGold, int, 6> {
  static constexpr int offset[6] = {0x19B8BE0, 0x10, 0x88, 0x28, 0x58, 0x1C4};
};

struct Knight {
  using GameElement = Typelist<KnightHealth, KnightPower, KnightGold>;
  void get_instance() {}

  const static std::string exeName;
  const static std::string dllName;
};

const std::string Knight::exeName = "hollow_knight.exe";
const std::string Knight::dllName = "UnityPlayer.dll";

#endif  // KNIGHT_HPP
