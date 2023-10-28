module;

#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>
#include <optional>
#include <vector>

export module Sat4;

import BaseSat;

namespace detailSat4 {
template <typename S_t> bool find(const S_t &S, std::vector<char> &VarSets);
}

export class Sat4_t final : public Sat_t {
  bool CanBeTrue;

  bool innerFind(std::vector<char> &VarSets) const override;

public:
  Sat4_t(int NewVarCount, Value_t &&NewValue, bool NewCanBeTrue = true)
      : Sat_t(NewVarCount, std::move(NewValue)), CanBeTrue(NewCanBeTrue) {
    analyze();
  }

  Sat setVar(int VarSet) const override;
  bool check() const override;
  void analyze() override;

  bool canBeTrue() const override { return CanBeTrue; }
};

Sat Sat4_t::setVar(int VarSet) const {
  // can be in [1, VarCount]");
  assert(this->VarCount > 0 && "Can't be zero VarCount!");

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

  bool OutputCanBeTrue =
      (std::find_if(Output.begin(), Output.end(), [](const auto &vec) {
         return vec.empty();
       }) == Output.end());

  return std::make_unique<Sat4_t>(this->VarCount - 1, std::move(Output),
                                  OutputCanBeTrue);
}

bool Sat4_t::check() const {
  if (!CanBeTrue)
    return false;
  if (this->VarCount == 0 || this->Value.empty())
    return true;

  for (int Var : this->getClause(0)) {
    if (this->setVar(Var)->check())
      return true;
  }
  return false;
}

bool Sat4_t::innerFind(std::vector<char> &VarSets) const {
  return detailSat4::find(this, VarSets);
}

// --------- DIFFER ---------
void Sat4_t::analyze() {
  std::ranges::stable_sort(
      this->Value,
      [](const auto &Lhs, const auto &Rhs) { return Lhs.size() < Rhs.size(); });
}
// --------- DIFFER ---------

template <typename S_t>
bool detailSat4::find(const S_t &S, std::vector<char> &VarSets) {
  if (!S->canBeTrue())
    return false;
  if (S->getVarCount() == 0 || S->isEmpty())
    return true;

  for (int Var : S->getClause(0)) {
    if (find(S->setVar(Var), VarSets)) {
      VarSets[std::abs(Var) - 1] = Var > 0;
      return true;
    }
  }

  return false;
}