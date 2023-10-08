import Sat;

import<iostream>;

int main() {
  Sat_t Sat({{1, 2, -3}, {-1, 2}});
  Sat.dump("Basic SAT");
  auto Output = Sat.setVar(-1);
  Output.dump("Set SAT -1");
}