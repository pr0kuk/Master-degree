module;

#include <bitset>

export module Algo2;

export import <optional>;
export import Sat2;

namespace detail {

bool simplestFind(const Sat2::Sat_t &Sat2, std::vector<char> &VarSets);

} // namespace detail

export namespace Algo2 {

bool simplestCheck(const Sat2::Sat_t &Sat2, size_t VarCount = 0);

std::optional<std::string> simplestFind(const Sat2::Sat_t &Sat2);

} // namespace Algo2

// -------------- Implementations ------------------------------------

bool Algo2::simplestCheck(const Sat2::Sat_t &Sat2, size_t VarCount) {
  // ---- DIFFER ----
  if (!Sat2.canBeTrue())
    return false;
  if (VarCount == 0)
    return Sat2;
  // ---- ------ ----
  return simplestCheck(Sat2.setVar(VarCount), VarCount - 1) ||
         simplestCheck(Sat2.setVar(-VarCount), VarCount - 1);
}

std::optional<std::string> Algo2::simplestFind(const Sat2::Sat_t &Sat2) {
  std::vector<char> VarSet(Sat2.getVarCount(), 0);
  if (!detail::simplestFind(Sat2, VarSet))
    return std::nullopt;

  std::string result = "";
  for (size_t i = 0; i < VarSet.size(); ++i) {
    result += VarSet[i] ? "x" : "~x";
    result += std::to_string(i + 1) + " ";
  }
  return result;
}

bool detail::simplestFind(const Sat2::Sat_t &Sat, std::vector<char> &VarSets) {
  // ---- DIFFER ----
  if (!Sat.canBeTrue())
    return false;
  // ---- ------ ----

  auto VarCount = Sat.getVarCount();
  if (VarCount == 0)
    return true;

  if (simplestFind(Sat.setLastVar(false), VarSets))
    return true;

  VarSets[VarCount - 1] = 1;
  if (simplestFind(Sat.setLastVar(true), VarSets))
    return true;
  VarSets[VarCount - 1] = 0;

  return false;
}