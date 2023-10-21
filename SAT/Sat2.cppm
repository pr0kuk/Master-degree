module;

#include <algorithm>

export module Sat2;

export import <iostream>;
export import <vector>;
import <functional>;

import Common;

namespace detail {
template <typename T>
concept is_convertible_str = std::is_convertible_v<T, std::string>;

template <is_convertible_str T> std::string baseJoin(T Elem) { return Elem; }
} // namespace detail

export namespace Sat2 {
using SatValue_t = std::vector<std::vector<int>>;
class Sat_t final {
  SatValue_t Value;
  // ---- DIFFER ----
  bool CanBeTrue;
  // ---- ------ ----
  static std::string toVariableDump(int Num);

public:
  Sat_t(SatValue_t &&NewValue, bool NewCanBeTrue = true)
      : Value(std::move(NewValue)), CanBeTrue(NewCanBeTrue) {}

  std::string dumpStr() const;
  void dump(std::string ExtraMsg = "Current SAT:") const;
  Sat_t setVar(int VarSet) const;

  operator bool() const { return CanBeTrue; }
};
} // namespace Sat2

// Implementations
// --------------------------------------------------------

std::string Sat2::Sat_t::toVariableDump(int Num) {
  std::string Out = Num < 0 ? "~" : "";
  return Out + "x" + std::to_string(std::abs(Num));
}

std::string Sat2::Sat_t::dumpStr() const {
  std::vector<std::string> Units;
  Units.reserve(Value.size());
  for (const auto &Conj : Value)
    Units.push_back(
        "( " + Common::join(Conj.begin(), Conj.end(), " | ", toVariableDump) +
        " )");

  return Common::join(Units.begin(), Units.end(), " & ",
                      detail::baseJoin<std::string>);
}
void Sat2::Sat_t::dump(std::string ExtraMsg) const {
  std::cout << ExtraMsg << "\n" << dumpStr() << "\n";
}

Sat2::Sat_t Sat2::Sat_t::setVar(int VarSet) const {
  SatValue_t Output;
  Output.reserve(Value.size());

  for (const auto &Clause : Value) {
    auto Iter = std::find(Clause.begin(), Clause.end(), VarSet);
    if (Iter != Clause.end())
      continue;
    std::vector<int> ClauseVars;
    std::copy_if(Clause.begin(), Clause.end(), std::back_inserter(ClauseVars),
                 [VarSet](int Var) { return Var != -VarSet; });
    Output.push_back(std::move(ClauseVars));
  }

  // ---- DIFFER ----
  bool OutputCanBeTrue =
      (std::find_if(Output.begin(), Output.end(), [](const auto &vec) {
         return vec.empty();
       }) == Output.end());
  // ---- ------ ----

  return Sat2::Sat_t(std::move(Output), OutputCanBeTrue);
}