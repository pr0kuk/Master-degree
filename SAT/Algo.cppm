export module Algo;

import Sat;

export namespace Algo {
bool simplestCheck(const Sat_t &Sat, size_t VarCount = 0) {
  if (VarCount == 0)
    return Sat;
  return simplestCheck(Sat.setVar(VarCount), VarCount - 1) ||
         simplestCheck(Sat.setVar(-VarCount), VarCount - 1);
}
} // namespace Algo