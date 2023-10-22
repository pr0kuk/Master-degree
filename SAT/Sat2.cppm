module;

#include <algorithm>
#include <cassert>

export module Sat2;

export import <iostream>;
export import <vector>;

import <fstream>;
import <functional>;
import <sstream>;
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
  int VarCount = 0;
  // ---- DIFFER ----
  bool CanBeTrue;
  // ---- ------ ----

  static std::string toVariableDump(int Num);

public:
  Sat_t(SatValue_t &&NewValue, int NewVarCount, bool NewCanBeTrue = true)
      : Value(std::move(NewValue)), VarCount(NewVarCount),
        CanBeTrue(NewCanBeTrue) {}

  int getVarCount() const { return VarCount; }
  operator bool() const { return Value.empty(); }
  bool canBeTrue() const { return CanBeTrue; }

  Sat_t setVar(int VarSet) const;
  Sat_t setLastVar(bool Var) const;

  void dump(std::string ExtraMsg = "Current SAT:") const;
  std::string dumpStr() const;
};

Sat2::Sat_t inputFromFile(std::string FileName) {
  std::ifstream In(FileName);
  SatValue_t Clauses;
  std::vector<int> Clause;
  std::string Line;
  bool Flag = false;
  int VarCount = 0;
  if (In.is_open()) {
    while (std::getline(In, Line) && !Flag) {
      if (Line[0] == 'c')
        continue;
      std::istringstream Ist(Line);
      if (Line[0] == 'p') {
        std::string Word;
        Ist >> Word;
        Ist >> Word;
        Ist >> Word;
        VarCount = atoi(Word.c_str());
        continue;
      }

      for (std::string Word; Ist >> Word;) {
        if (Word == "%")
          Flag = true;
        if (Word != "0")
          Clause.push_back(atoi(Word.c_str()));
        else {
          Clauses.push_back(Clause);
          Clause.clear();
        }
      }
    }
  }
  In.close();
  Sat2::Sat_t Sat(std::move(Clauses), VarCount);
  return Sat;
}
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
  // assert(VarCount > 0 && VarSet > 0 && VarSet <= VarCount && "Set variable
  // can be in [1, VarCount]");

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

  return Sat2::Sat_t(std::move(Output), VarCount - 1, OutputCanBeTrue);
}

Sat2::Sat_t Sat2::Sat_t::setLastVar(bool Var) const {
  assert(VarCount > 0 && "VarCount can't be 0!");
  return setVar(Var ? VarCount : -VarCount);
}