module;

#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>
#include <optional>
#include <vector>

export module Sat3;

export import BaseSat;

namespace detailSat3 {
template <typename S_t> bool find(const S_t &S, std::vector<char> &VarSets);
}

export class Sat3_t final : public Sat_t {
  bool CanBeTrue;

public:
  Sat3_t(int NewVarCount, Value_t &&NewValue, bool NewCanBeTrue = true)
      : Sat_t(NewVarCount, std::move(NewValue)), CanBeTrue(NewCanBeTrue) {}

  Sat setVar(int VarSet) const override;
  bool check() const override;
  std::optional<std::string> find() const override;

  bool canBeTrue() const override { return CanBeTrue; }
};

Sat Sat3_t::setVar(int VarSet) const {
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

  return std::make_unique<Sat3_t>(this->VarCount - 1, std::move(Output),
                                  OutputCanBeTrue);
}

bool Sat3_t::check() const {
  if (!CanBeTrue)
    return false;
  // ---- DIFFER ----
  if (this->VarCount == 0 || this->Value.empty())
    return true;
  for (int Var : this->getClause(0)) {
    if (this->setVar(Var)->check())
      return true;
  }
  // ---- ------ ----
  return false;
}

std::optional<std::string> Sat3_t::find() const {
  std::vector<char> VarSets(this->VarCount, 0);
  if (!detailSat3::find(this, VarSets))
    return std::nullopt;

  std::string result = "";
  for (int i = 0; i < VarSets.size(); ++i) {
    result += VarSets[i] ? "x" : "~x";
    result += std::to_string(i + 1) + " ";
  }
  return result;
}

template <typename S_t>
bool detailSat3::find(const S_t &S, std::vector<char> &VarSets) {
  if (!S->canBeTrue())
    return false;

  // ---- DIFFER ----
  if (S->getVarCount() == 0 || S->isEmpty())
    return true;

  for (int Var : S->getClause(0)) {
    if (find(S->setVar(Var), VarSets)) {
      VarSets[Var - 1] = Var > 0;
      return true;
    }
  }
  // ---- ------ ----

  return false;
}