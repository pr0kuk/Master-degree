#include <iostream>
#include <vector>
#include <concepts>
#include <functional>

using sat_t = std::vector<std::vector<int>>;

template <typename T>
concept ConvertibleToString = requires(T A) { std::to_string(A); };
template <typename T>
concept IterConvertibleToString = ConvertibleToString<typename T::value_type>;

std::string variable(int num) {
    std::string out = num < 0 ? "~" : "";
    return out + "x" + std::to_string(std::abs(num));
}

template <typename Iter>
std::string join(Iter Begin, Iter End, const std::string &Separator, std::function<std::string(typename Iter::value_type)> func = [](typename Iter::value_type Elem) {return Elem;})
{
  std::string Result;
  if (Begin != End) {
    Result += func(*Begin);
    ++Begin;
  }
  while (Begin != End) {
    Result += Separator + func(*Begin);
    ++Begin;
  }
  return Result;
}

void dump_sat(const sat_t &sat, std::string extra_msg = "Current SAT:") {
    std::cout << extra_msg << "\n";
    std::vector<std::string> Units;
    Units.reserve(sat.size());

    for (const auto & conj : sat)
        Units.push_back("( " + join(conj.begin(), conj.end(), " | ", variable) + " )");
    
    std::cout << join(Units.begin(), Units.end(), " & ");
}

int main() {
    sat_t sat = {
        {1, 2, -3},
        {-1, 2}
    };
    dump_sat(sat);
}