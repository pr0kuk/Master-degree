module;

#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>
#include <optional>
#include <vector>

export module Sat1;

import BaseSat;

namespace detailSat1 {
template <typename S_t> bool find(const S_t &S, std::vector<char> &VarSets);
}

export class Sat1_t final : public Sat_t {
  bool innerFind(std::vector<char> &VarSets) const override;

public:
  Sat1_t(int NewVarCount, Value_t &&NewValue)
      : Sat_t(NewVarCount, std::move(NewValue)) {}

  Sat setVar(int VarSet) const override;
  bool check() const override;
};

Sat Sat1_t::setVar(int VarSet) const {
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
  return std::make_unique<Sat1_t>(VarCount - 1, std::move(Output));
}

bool Sat1_t::check() const {
  if (this->VarCount == 0)
    return *this;
  return setMostImportantVar(true)->check() ||
         setMostImportantVar(false)->check();
}

bool Sat1_t::innerFind(std::vector<char> &VarSets) const {
  return detailSat1::find(this, VarSets);
}

template <typename S_t>
bool detailSat1::find(const S_t &S, std::vector<char> &VarSets) {
  if (S->getVarCount() == 0)
    return S->isEmpty();

  if (find(S->setMostImportantVar(false), VarSets))
    return true;

  VarSets[S->getVarCount() - 1] = 1;
  if (find(S->setMostImportantVar(true), VarSets))
    return true;
  VarSets[S->getVarCount() - 1] = 0;

  return false;
}