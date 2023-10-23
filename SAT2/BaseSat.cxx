// Global module fragment where #includes can happen
module;

#include <cassert>
#include <functional>
#include <iostream>
#include <memory>
#include <vector>

// first thing after the Global module fragment must be a module command
export module BaseSat;

namespace detail {
template <typename T>
concept is_convertible_str = std::is_convertible_v<T, std::string>;
template <is_convertible_str T> std::string baseJoin(T Elem) { return Elem; }
std::string varJoin(int Num);

template <typename Iter, typename F>
std::string join(Iter Begin, Iter End, const std::string &Separator, F func);
} // namespace detail

export class Sat_t;
export using Sat = std::unique_ptr<Sat_t>;

class Sat_t {
protected:
  using Value_t = std::vector<std::vector<int>>;

  int VarCount;
  Value_t Value;

public:
  Sat_t(int NewVarCount, Value_t &&NewValue)
      : VarCount(NewVarCount), Value(NewValue) {}
  virtual ~Sat_t() {}

  // to implement by optimizations
  virtual Sat setVar(int VarSet) const = 0;
  Sat setLastVar(bool Var) const;

  int getVarCount() const { return VarCount; }
  operator bool() const { return Value.empty(); }

  std::string dumpStr() const;
  void dump(std::string ExtraMsg = "Current SAT:") const;
};

Sat Sat_t::setLastVar(bool Var) const {
  assert(VarCount > 0 && "VarCount can't be 0!");
  return this->setVar(Var ? VarCount : -VarCount);
}

std::string Sat_t::dumpStr() const {
  std::vector<std::string> Units;
  Units.reserve(Value.size());
  for (const auto &Conj : Value)
    Units.push_back(
        "( " + detail::join(Conj.begin(), Conj.end(), " | ", detail::varJoin) +
        " )");

  return detail::join(Units.begin(), Units.end(), " & ",
                      detail::baseJoin<std::string>);
}
void Sat_t::dump(std::string ExtraMsg) const {
  std::cout << ExtraMsg << "\n" << dumpStr() << "\n";
}

std::string detail::varJoin(int Num) {
  std::string Out = Num < 0 ? "~" : "";
  return Out + "x" + std::to_string(std::abs(Num));
}
template <typename Iter, typename F>
std::string detail::join(Iter Begin, Iter End, const std::string &Separator,
                         F func) {
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