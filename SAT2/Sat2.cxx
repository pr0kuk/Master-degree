module;

#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>
#include <optional>
#include <vector>

export module Sat2;

export import BaseSat;

namespace detailSat2 {
template <typename S_t> bool find(const S_t &S, std::vector<char> &VarSets);
}

export class Sat2_t final : public Sat_t {
  // ---- DIFFER ----
  bool CanBeTrue;
  // ---- ------ ----

  bool innerFind(std::vector<char> &VarSets) const override;

public:
  Sat2_t(int NewVarCount, Value_t &&NewValue, bool NewCanBeTrue = true)
      : Sat_t(NewVarCount, std::move(NewValue)), CanBeTrue(NewCanBeTrue) {}

  Sat setVar(int VarSet) const override;
  bool check() const override;

  bool canBeTrue() const override { return CanBeTrue; }
};

Sat Sat2_t::setVar(int VarSet) const {
  // can be in [1, VarCount]");
  assert(this->VarCount > 0 && "Can't be zero VarCount!");
  assert(std::abs(VarSet) <= this->VarCount &&
         "Set variable in range [+-1, ..., +-VarCount]!");

  Value_t Output;
  Output.reserve(this->Value.size());
  for (const auto &Clause : this->Value) {
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

  return std::make_unique<Sat2_t>(this->VarCount - 1, std::move(Output),
                                  OutputCanBeTrue);
}

bool Sat2_t::check() const {
  // ---- DIFFER ----
  if (!CanBeTrue)
    return false;
  if (this->VarCount == 0)
    return true;
  // ---- ------ ----
  return setLastVar(true)->check() || setLastVar(false)->check();
}

bool Sat2_t::innerFind(std::vector<char> &VarSets) const {
  return detailSat2::find(this, VarSets);
}

template <typename S_t>
bool detailSat2::find(const S_t &S, std::vector<char> &VarSets) {
  // ---- DIFFER ----
  if (!S->canBeTrue())
    return false;
  // ---- ------ ----

  if (S->getVarCount() == 0)
    return S->isEmpty();

  if (find(S->setLastVar(false), VarSets))
    return true;

  VarSets[S->getVarCount() - 1] = 1;
  if (find(S->setLastVar(true), VarSets))
    return true;
  VarSets[S->getVarCount() - 1] = 0;

  return false;
}