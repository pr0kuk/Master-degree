import <iostream>;
import <string>;
import Sat;
import Algo;

template <class T>
concept equality_comparable = requires(T a, T b) {
  a == b;
};

template <equality_comparable T>
bool compare(T Lhs, T Rhs, size_t Line, const char Filename[]) {
  if (Lhs == Rhs) {
    std::cout << "\033[1;32mPASSED!\033[0m\n";
    return true;
  }
  std::cout << "\033[1;31mTEST FAILED!\n";
  std::cout << "Where: " << Filename << ":" << Line << std::endl;
  std::cout << "\033[1;34mLhs: " << Lhs << "\nRhs: " << Rhs << "\033[0m"
            << std::endl;
  return false;
}

#define EXPECT_EQ(lhs, rhs) compare(lhs, rhs, __LINE__, __FILE__)

void testSetVar() {
  Sat_t Sat({{1, 2, -3}, {-1, 2}});
  // ( x1 | x2 | ~x3 ) & ( ~x1 | x2 )

  auto Output = Sat.setVar(-1);
  EXPECT_EQ(Output.dumpStr(), std::string("( x2 | ~x3 )"));
  Output = Sat.setVar(1);
  EXPECT_EQ(Output.dumpStr(), std::string("( x2 )"));
  Output = Sat.setVar(2);
  EXPECT_EQ(Output.dumpStr(), std::string(""));
  Output = Sat.setVar(3);
  EXPECT_EQ(Output.dumpStr(), std::string("( x1 | x2 ) & ( ~x1 | x2 )"));
}

void testSimplestCheck() {
  Sat_t SatTrue({{1, 2, -3}, {-1, 2}});
  Sat_t SatFalse({{1}, {-1}});
  // ( x1 | x2 | ~x3 ) & ( ~x1 | x2 )

  EXPECT_EQ(Algo::simplestCheck(SatTrue, 3), true);
  EXPECT_EQ(Algo::simplestCheck(SatFalse, 1), false);
}

void testSimplestFind() { // to implement
  // Sat_t SatTrue({{1, 2, -3}, {-1, 2}});
  // Sat_t SatFalse({{1}, {-1}});
  //// ( x1 | x2 | ~x3 ) & ( ~x1 | x2 )
  //
  // EXPECT_EQ(Algo::simplestCheck(SatTrue, 3), true);
  // EXPECT_EQ(Algo::simplestCheck(SatFalse, 1), false);
}

int main(int argc, char **argv) {
  testSetVar();
  testSimplestCheck();
  testSimplestFind();
}