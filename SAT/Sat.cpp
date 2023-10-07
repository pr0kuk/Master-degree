#include "Sat.h"

std::string Sat_t::toVariableDump(int Num) {
  std::string Out = Num < 0 ? "~" : "";
  return Out + "x" + std::to_string(std::abs(Num));
}

void Sat_t::dump(std::string ExtraMsg) const {
  std::cout << ExtraMsg << "\n" 
    << dumpStr() << "\n";
}

std::string Sat_t::dumpStr() const {
  std::vector<std::string> Units;
  Units.reserve(Value.size());
  for (const auto &Conj : Value)
    Units.push_back(
        "( " + join(Conj.begin(), Conj.end(), " | ", toVariableDump) + " )");

  return join(Units.begin(), Units.end(), " & ");
}

Sat_t Sat_t::setVar(int VarSet) const {
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