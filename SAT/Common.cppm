export module Common;

import <iostream>;
import <functional>;

export namespace Common {
template <typename Iter, typename F>
std::string join(Iter Begin, Iter End, const std::string &Separator, F func);
}

template <typename Iter, typename F>
std::string Common::join(Iter Begin, Iter End, const std::string &Separator,
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