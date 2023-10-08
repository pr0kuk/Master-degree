export module Sat;

import<algorithm>;
import<functional>;
import<iostream>;
export import<vector>;
import<string>;

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

export using SatValue_t = std::vector<std::vector<int>>;
export class Sat_t final {
  SatValue_t Value;

  static std::string toVariableDump(int Num);

public:
  Sat_t(SatValue_t &&NewValue) : Value(std::move(NewValue)) {}

  std::string dumpStr() const {
    std::vector<std::string> Units;
    Units.reserve(Value.size());
    for (const auto &Conj : Value)
      Units.push_back(
          "( " + join(Conj.begin(), Conj.end(), " | ", toVariableDump) + " )");

    return join(Units.begin(), Units.end(), " & ");
  }
  void dump(std::string ExtraMsg = "Current SAT:") const {
    std::cout << ExtraMsg << "\n" 
      << dumpStr() << "\n";
  }
  Sat_t setVar(int VarSet) const {
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
    return Output;
  }

};

std::string Sat_t::toVariableDump(int Num) {
  std::string Out = Num < 0 ? "~" : "";
  return Out + "x" + std::to_string(std::abs(Num));
}