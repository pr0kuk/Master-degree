export module Common;

import <iostream>;
import <functional>;

export template <typename Iter>
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