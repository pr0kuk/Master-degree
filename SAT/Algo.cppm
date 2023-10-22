module;

#include <bitset>

export module Algo;

export import <optional>;
export import Sat;

namespace detail {

// returns true if found
bool simplestFind(const Sat::Sat_t &Sat, std::vector<char> &VarSets);

} // namespace detail

export namespace Algo {

bool simplestCheck(const Sat::Sat_t &Sat, size_t VarCount = 0);

std::optional<std::string> simplestFind(const Sat::Sat_t &Sat);

} // namespace Algo

// -------------- Implementations ------------------------------------

bool Algo::simplestCheck(const Sat::Sat_t &Sat, size_t VarCount) {
  if (VarCount == 0)
    return Sat;
  return simplestCheck(Sat.setVar(VarCount), VarCount - 1) ||
         simplestCheck(Sat.setVar(-VarCount), VarCount - 1);
}

std::optional<std::string> Algo::simplestFind(const Sat::Sat_t &Sat) {
  std::vector<char> VarSet(Sat.getVarCount(), 0);
  if (!detail::simplestFind(Sat, VarSet))
    return std::nullopt;

  std::string result = "";
  for (int i = 0; i < VarSet.size(); ++i) {
    result += VarSet[i] ? "x" : "~x";
    result += std::to_string(i + 1) + " ";
  }
  return result;
}

bool detail::simplestFind(const Sat::Sat_t &Sat, std::vector<char> &VarSets) {
  auto VarCount = Sat.getVarCount();
  if (VarCount == 0)
    return Sat;

  if (simplestFind(Sat.setLastVar(false), VarSets))
    return true;

  VarSets[VarCount - 1] = 1;
  if (simplestFind(Sat.setLastVar(true), VarSets))
    return true;
  VarSets[VarCount - 1] = 0;

  return false;
}