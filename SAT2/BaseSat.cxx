// Global module fragment where #includes can happen
module;

#include <cassert>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <vector>

// first thing after the Global module fragment must be a module command
export module BaseSat;

namespace detail {
std::string strConvert(std::string Str);
std::string intConvert(int Num);

template <typename Iter, typename F>
std::string join(Iter Begin, Iter End, std::string Separator, F func);

template <typename Iter>
std::string intJoin(Iter Begin, Iter End, std::string Separator);
template <typename Iter>
std::string strJoin(Iter Begin, Iter End, std::string Separator);
} // namespace detail

export class Sat_t;
export using Sat = std::unique_ptr<Sat_t>;

class Sat_t {
protected:
  using Value_t = std::vector<std::vector<int>>;

  int VarCount;
  Value_t Value;

  virtual bool innerFind(std::vector<char> &) const = 0;

public:
  Sat_t(int NewVarCount, Value_t &&NewValue)
      : VarCount(NewVarCount), Value(NewValue) {}
  virtual ~Sat_t() {}

  // to implement by optimizations
  virtual Sat setVar(int VarSet) const = 0;
  virtual Sat setMostImportantVar(bool Var) const;

  virtual bool check() const = 0;
  std::optional<std::string> find() const;
  virtual void analyze() {}

  int getVarCount() const { return VarCount; }
  std::vector<int> getClause(int N) const { return Value[N]; }
  virtual int getMostImportantVar() const { return VarCount; }

  bool isEmpty() const { return Value.empty(); }
  virtual bool canBeTrue() const { return isEmpty() || (VarCount == 0); }
  virtual operator bool() const { return isEmpty(); }

  std::string dumpStr() const;
  void dump(std::string ExtraMsg = "Current SAT:") const;
};

Sat Sat_t::setMostImportantVar(bool Var) const {
  assert(VarCount > 0 && "VarCount can't be 0!");
  return this->setVar(Var ? VarCount : -VarCount);
}

std::optional<std::string> Sat_t::find() const {
  std::vector<char> VarSets(this->VarCount, 0);
  if (!this->innerFind(VarSets))
    return std::nullopt;

  std::vector<std::string> Result;
  for (int i = 0; i < VarSets.size(); ++i) {
    std::string BoolVar = VarSets[i] ? "" : "~";
    Result.push_back(BoolVar + "x" + std::to_string(i + 1));
  }

  return detail::strJoin(Result.begin(), Result.end(), " ");
}

std::string Sat_t::dumpStr() const {
  std::vector<std::string> Units;
  Units.reserve(Value.size());
  for (const auto &Conj : Value)
    Units.push_back("( " + detail::intJoin(Conj.begin(), Conj.end(), " | ") +
                    " )");

  return detail::strJoin(Units.begin(), Units.end(), " & ");
}
void Sat_t::dump(std::string ExtraMsg) const {
  std::cout << ExtraMsg << "\n" << dumpStr() << "\n";
}

namespace detail {
std::string strConvert(std::string Str) { return Str; }
std::string intConvert(int Num) {
  std::string Out = Num < 0 ? "~" : "";
  return Out + "x" + std::to_string(std::abs(Num));
}

template <typename Iter, typename F>
std::string join(Iter Begin, Iter End, std::string Separator, F func) {
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

template <typename Iter>
std::string intJoin(Iter Begin, Iter End, std::string Separator) {
  return join(Begin, End, std::move(Separator), intConvert);
}
template <typename Iter>
std::string strJoin(Iter Begin, Iter End, std::string Separator) {
  return join(Begin, End, std::move(Separator), strConvert);
}
} // namespace detail