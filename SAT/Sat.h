#pragma once
#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>

template <typename Iter>
std::string join(
    Iter Begin, Iter End, const std::string &Separator,
    std::function<std::string(typename Iter::value_type)> func =
        [](typename Iter::value_type Elem) { return Elem; }) {
  if (Begin == End)
    return std::string{};
  std::string Result = func(*Begin);
  ++Begin;
  while (Begin != End) {
    Result += Separator + func(*Begin);
    ++Begin;
  }
  return Result;
}

using SatValue_t = std::vector<std::vector<int>>;
class Sat_t final {

  SatValue_t Value;

  static std::string toVariableDump(int Num);

public:
  Sat_t(SatValue_t &&NewValue) : Value(std::move(NewValue)) {}

  std::string dumpStr() const;
  void dump(std::string ExtraMsg = "Current SAT:") const;
  Sat_t setVar(int VarSet) const;

};