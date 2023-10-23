module;

#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>
#include <optional>
#include <vector>

export module Sat;

export import BaseSat;

namespace detail {
template <typename S_t> bool find(const S_t &S, std::vector<char> &VarSets);
}

export class Sat1_t final : public Sat_t {
public:
  Sat1_t(int NewVarCount, Value_t &&NewValue)
      : Sat_t(NewVarCount, std::move(NewValue)) {}

  Sat setVar(int VarSet) const override;
  bool check() const override;
  std::optional<std::string> find() const override;
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
  return setLastVar(true)->check() || setLastVar(false)->check();
}

std::optional<std::string> Sat1_t::find() const {
  std::vector<char> VarSets(this->VarCount, 0);
  if (!detail::find(this, VarSets))
    return std::nullopt;

  std::string result = "";
  for (int i = 0; i < VarSets.size(); ++i) {
    result += VarSets[i] ? "x" : "~x";
    result += std::to_string(i + 1) + " ";
  }
  return result;
}

template <typename S_t>
bool detail::find(const S_t &S, std::vector<char> &VarSets) {
  if (S->getVarCount() == 0)
    return *S;

  if (find(S->setLastVar(false), VarSets))
    return true;

  VarSets[S->getVarCount() - 1] = 1;
  if (find(S->setLastVar(true), VarSets))
    return true;
  VarSets[S->getVarCount() - 1] = 0;

  return false;
}