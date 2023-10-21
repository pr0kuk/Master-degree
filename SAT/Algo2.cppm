module;

#include <bitset>

export module Algo2;

export import <optional>;
export import Sat2;

namespace detail {

template <size_t N>
std::optional<std::bitset<N>>
simplestFind(const Sat2::Sat_t &Sat2, size_t VarCount = N,
             std::bitset<N> VarSets = std::bitset<N>{});

} // namespace detail

export namespace Algo2 {

bool simplestCheck(const Sat2::Sat_t &Sat2, size_t VarCount = 0);

template <size_t N>
std::optional<std::string> simplestFind(const Sat2::Sat_t &Sat2);

} // namespace Algo2

// -------------- Implementations ------------------------------------

bool Algo2::simplestCheck(const Sat2::Sat_t &Sat2, size_t VarCount) {
  // ---- DIFFER ----
  if (!Sat2 || VarCount == 0)
    return Sat2;
  // ---- ------ ----
  return simplestCheck(Sat2.setVar(VarCount), VarCount - 1) ||
         simplestCheck(Sat2.setVar(-VarCount), VarCount - 1);
}

template <size_t N>
std::optional<std::string> Algo2::simplestFind(const Sat2::Sat_t &Sat2) {
  auto VarSet = detail::simplestFind<N>(Sat2);
  if (!VarSet)
    return std::nullopt;
  std::string result = "";
  for (size_t i = 0; i < N; ++i) {
    result += VarSet->test(i) ? "x" : "~x";
    result += std::to_string(i + 1) + " ";
  }
  return result;
}

template <size_t N>
std::optional<std::bitset<N>> detail::simplestFind(const Sat2::Sat_t &Sat2,
                                                   size_t VarCount,
                                                   std::bitset<N> VarSets) {
  // ---- DIFFER ----
  if (!Sat2)
    return std::nullopt;
  // ---- ------ ----

  if (VarCount == 0)
    return std::make_optional(VarSets);

  if (auto Set = simplestFind(Sat2.setVar(-VarCount), VarCount - 1, VarSets))
    return Set;

  std::bitset<N> NextVarSets = VarSets;
  NextVarSets.set(VarCount - 1);

  if (auto Set = simplestFind(Sat2.setVar(VarCount), VarCount - 1, NextVarSets))
    return Set;

  return std::nullopt;
}