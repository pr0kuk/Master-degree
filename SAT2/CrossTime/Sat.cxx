// Global module fragment where #includes can happen
module;

#include <algorithm>
#include <array>
#include <cassert>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <vector>

// first thing after the Global module fragment must be a module command
export module Sat;

export enum class SAT_STATE { TRUE, FALSE, INVEST };

export template <int ClauseCount> class Sat_t {
  std::array<std::array<int, 3>, ClauseCount> Data;
  const int VarCount;

  constexpr int setClause(std::array<int, 3> &NewData, int Var);

public:
  constexpr Sat_t(std::array<std::array<int, 3>, ClauseCount> NewData,
                   int NewVarCount)
      : Data(NewData), VarCount(NewVarCount) {}

  constexpr int getNextClauseCount(int Var) const;
  constexpr int getNextClauseCount(const std::array<int, 3> &data, int Var) const;

  template <int NewClauseCount>
  constexpr Sat_t<NewClauseCount> setVar(int Var) const;
  constexpr Sat_t<NewClauseCount> setLastVar(bool True) const {
    return True ? setVar(VarCount) : setVar(-VarCount);
  }
  constexpr bool check() const;

  constexpr SAT_STATE getState() const;
  constexpr operator bool() const { return ImportantClause != -1; }

  constexpr std::array<int, 3> operator[](int I) const { return Data[I]; }
  constexpr std::array<std::array<int, 3>, ClauseCount> getData() const {
    return Data;
  }
  constexpr int getImportantClause() const { return ImportantClause; }
};

template <int ClauseCount>
constexpr int Sat_t<ClauseCount>::setClause(std::array<int, 3> &NewData, int Var) {
  if (std::find(NewData.cbegin(), NewData.cend(), Var) != NewData.end()) {
    std::fill(NewData.begin(), NewData.end(), 0);
    return 1;
  }
  if (std::all_of(NewData.cbegin(), NewData.cend(),
                  [](int i) { return i == 0; }))
    return 0;

  std::transform(NewData.begin(), NewData.end(), NewData.begin(),
                 [Var](int Num) { return Num != -Var ? Num : 0; });

  if (std::all_of(NewData.cbegin(), NewData.cend(),
                  [](int i) { return i == 0; }))
    return -1;

  return 0;
}

template <int ClauseCount>
constexpr Sat_t<ClauseCount> Sat_t<ClauseCount>::setVar(int Var) const {
  std::array<std::array<int, 3>, ClauseCount> NewData(Data);
  int NewImpClause = ImportantClause;
  for (auto &Dat : NewData) {
    int Check = setClause(Dat, Var);
    if (Check == -1) {
      NewImpClause = -1;
      break;
    }
    NewImpClause -= Check;
  }
  return Sat1_t<ClauseCount>(NewData, VarCount - 1, NewImpClause);
}

template <int ClauseCount> consteval bool Sat1_t<ClauseCount>::check() const {
  switch (getState()) {
  case SAT_STATE::TRUE:
    return true;
  case SAT_STATE::FALSE:
    return false;
  default:
    break;
  }
  return setLastVar(true).check() || setLastVar(false).check();
}

template <int ClauseCount>
constexpr int Sat_t<ClauseCount>::getNextClauseCount(const std::array<int, 3> &Data, int Var) const {
  if (std::find(Data.cbegin(), Data.cend(), Var) != Data.end())
    return 1;
  if (std::all_of(Data.cbegin(), Data.cend(),
                  [Var](int i) { return i == 0 || i == -Var; }))
    return -1;
  return 0;
}

template <int ClauseCount>
constexpr int Sat_t<ClauseCount>::getNextClauseCount(int Var) const {
  int Output = ClauseCount;
  for (const auto &Dat : Data) {
    int Check = analyzeClause(Dat, Var);
    if (Check == -1)
      return -1;
    Output -= Check;
  }
  return Output;
}

template <int ClauseCount>
consteval SAT_STATE Sat1_t<ClauseCount>::getState() const {
  switch (ImportantClause) {
  case -1:
    return SAT_STATE::FALSE;
  case 0:
    return SAT_STATE::TRUE;
  }
  return SAT_STATE::INVEST;
}