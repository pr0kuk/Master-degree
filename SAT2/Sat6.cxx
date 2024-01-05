module;

#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <memory>
#include <optional>
#include <vector>

export module Sat6;

import BaseSat;

namespace detailSat6 {
template <typename S_t> bool find(const S_t &S, std::vector<char> &VarSets);
}

export class Sat6_t final : public Sat_t {
  bool CanBeTrue;
  int MostImportantVar;
  int FullCount;

  bool innerFind(std::vector<char> &VarSets) const override;

public:
  Sat6_t(int NewVarCount, Value_t &&NewValue, bool NewCanBeTrue = true,
         int NewFullCount = -1)
      : Sat_t(NewVarCount, std::move(NewValue)), CanBeTrue(NewCanBeTrue),
        MostImportantVar(NewVarCount), FullCount(NewFullCount) {
    if (FullCount < 0)
      FullCount = VarCount;
    analyze();
  }

  Sat setVar(int VarSet) const override;
  Sat setMostImportantVar(bool VarSet) const override;
  bool check() const override;
  void analyze() override;

  bool canBeTrue() const override { return CanBeTrue; }
  int getMostImportantVar() const override { return MostImportantVar; }
};

Sat Sat6_t::setMostImportantVar(bool VarSet) const {
  return this->setVar(VarSet ? this->MostImportantVar
                             : -1 * this->MostImportantVar);
}

Sat Sat6_t::setVar(int VarSet) const {
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

  return std::make_unique<Sat6_t>(this->VarCount - 1, std::move(Output),
                                  OutputCanBeTrue, this->FullCount);
}

bool Sat6_t::check() const {
  // ---- DIFFER ----
  if (!CanBeTrue)
    return false;
  if (this->VarCount == 0)
    return true;
  // ---- ------ ----
  return this->setMostImportantVar(true)->check() ||
         this->setMostImportantVar(false)->check();
}

bool Sat6_t::innerFind(std::vector<char> &VarSets) const {
  return detailSat6::find(this, VarSets);
}

// --------- DIFFER ---------
void Sat6_t::analyze() {
  if (!this->VarCount)
    return;

  std::vector<int> Weights(FullCount, 0);
  std::vector<std::array<int, 2>> Count(FullCount, {0, 0});
  for (const auto &Vec : this->Value) {
    int Coef = (3 - Vec.size()) * 5;
    for (auto Elem : Vec) {
      auto Index = std::abs(Elem) - 1;
      Weights[Index] += 1 << Coef;
      ++Count[Index][Elem > 0];
    }
  }

  // std::cout << "Weights: ";
  // for (auto Elem : Weights) {
  //   std::cout << Elem << " ";
  // }
  // std::cout << "\nPositive: ";
  // for (auto Elem : Positive) {
  //   std::cout << Elem << " ";
  // }
  // std::cout << "\n";
  auto Index = std::distance(Weights.begin(),
                             std::max_element(Weights.begin(), Weights.end()));
  MostImportantVar = Index + 1;
  if (Count[Index][0] > Count[Index][1])
    MostImportantVar *= -1;
}
// --------- DIFFER ---------

template <typename S_t>
bool detailSat6::find(const S_t &S, std::vector<char> &VarSets) {
  // ---- DIFFER ----
  if (!S->canBeTrue())
    return false;
  // ---- ------ ----

  if (S->getVarCount() == 0)
    return S->isEmpty();

  auto MostImVar = S->getMostImportantVar();
  // std::cout << "Up: " << MostImVar << "\n";

  if (find(S->setMostImportantVar(true), VarSets)) {
    VarSets[std::abs(MostImVar) - 1] = MostImVar > 0;
    return true;
  }

  // std::cout << "Down: "<< -MostImVar << "\n";

  if (find(S->setMostImportantVar(false), VarSets)) {
    VarSets[std::abs(MostImVar) - 1] = MostImVar < 0;
    return true;
  }

  return false;
}