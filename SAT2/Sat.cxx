module;

#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

export module Sat;

export import BaseSat;

export class Sat1_t final : public Sat_t {
public:
  Sat1_t(int NewVarCount, Value_t &&NewValue)
      : Sat_t(NewVarCount, std::move(NewValue)) {}

  Sat setVar(int VarSet) const override;
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