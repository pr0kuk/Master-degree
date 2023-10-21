import Algo;
import Algo2;

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
bool compare(bool Cmp, size_t Line, const char Filename[]) {
  if (Cmp) {
    std::cout << "\033[1;32mPASSED!\033[0m\n";
    return true;
  }
  std::cout << "\033[1;31mTEST FAILED!\n";
  std::cout << "Where: " << Filename << ":" << Line << std::endl;
  return false;
}

#define EXPECT_EQ(lhs, rhs) compare(lhs, rhs, __LINE__, __FILE__)
#define EXPECT_SET(cmp) compare(cmp, __LINE__, __FILE__)

void testSetVar() {
  Sat::Sat_t Sat({{1, 2, -3}, {-1, 2}});
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
  Sat::Sat_t SatTrue({{1, 2, -3}, {-1, 2}});
  Sat::Sat_t SatFalse({{1}, {-1}});
  // ( x1 | x2 | ~x3 ) & ( ~x1 | x2 )

  EXPECT_EQ(Algo::simplestCheck(SatTrue, 3), true);
  EXPECT_EQ(Algo::simplestCheck(SatFalse, 1), false);
}

void testFileInput() {
  Sat::Sat_t SatTrue = Sat::inputFromFile("cnf/manual.cnf");
  EXPECT_EQ(SatTrue.dumpStr(), std::string("( x1 | x2 | ~x3 ) & ( ~x1 | x2 )"));
  EXPECT_EQ(Algo::simplestCheck(SatTrue, 3), true);
}

void testUf() {
  Sat::Sat_t SatTrue = Sat::inputFromFile("cnf/uf20-01.cnf");
  Sat2::Sat_t SatTrue2 = Sat2::inputFromFile("cnf/uf20-01.cnf");
  EXPECT_EQ(Algo::simplestCheck(SatTrue, 3), true);
  auto Set1 = Algo::simplestFind<3>(SatTrue);
  EXPECT_EQ(Set1.has_value(), true);
  auto Set2 = Algo2::simplestFind<3>(SatTrue2);
  EXPECT_EQ(Algo2::simplestCheck(SatTrue2, 3), true);
  EXPECT_EQ(Set2.has_value(), true);
  //SatTrue.dump();
  //SatTrue2.dump();
}

void testSimplestFind() { // to implement
  Sat::Sat_t SatFalse({{1}, {-1}});
  auto SetFalse = Algo::simplestFind<1>(SatFalse);
  Sat::Sat_t Sat1({{1, 2, -3}, {-1, 2}});
  auto Set1 = Algo::simplestFind<3>(Sat1);
  Sat::Sat_t Sat2({{1, 2, -3}, {1, 2}});
  auto Set2 = Algo::simplestFind<3>(Sat2);

  EXPECT_EQ(SetFalse.has_value(), false);
  EXPECT_EQ(Set1.has_value(), true);
  EXPECT_EQ(*Set1, std::string("~x1 ~x2 ~x3 "));
  EXPECT_EQ(*Set2, std::string("x1 ~x2 ~x3 "));

  Sat::Sat_t Sat3({{1, -2}, {1, 2}});
  EXPECT_EQ(*Algo::simplestFind<2>(Sat3), std::string("x1 ~x2 "));
  Sat::Sat_t Sat4({{1, 2}, {-1, 2}});
  EXPECT_EQ(*Algo::simplestFind<2>(Sat4), std::string("~x1 x2 "));
}

void testSimplestFind2() { // to implement
  Sat2::Sat_t SatFalse({{1}, {-1}});
  auto SetFalse = Algo2::simplestFind<1>(SatFalse);
  EXPECT_EQ(SetFalse.has_value(), false);

  Sat2::Sat_t Sat1({{1, 2, -3}, {-1, 2}});
  auto Set1 = Algo2::simplestFind<3>(Sat1);
  EXPECT_EQ(Set1.has_value(), true);
  EXPECT_EQ(*Set1, std::string("~x1 ~x2 ~x3 "));

  Sat2::Sat_t Sat2({{1, 2, -3}, {1, 2}});
  auto Set2 = Algo2::simplestFind<3>(Sat2);
  EXPECT_EQ(*Set2, std::string("x1 ~x2 ~x3 "));

  Sat2::Sat_t Sat3({{1, -2}, {1, 2}});
  EXPECT_EQ(*Algo2::simplestFind<2>(Sat3), std::string("x1 ~x2 "));

  Sat2::Sat_t Sat4({{1, 2}, {-1, 2}});
  EXPECT_EQ(*Algo2::simplestFind<2>(Sat4), std::string("~x1 x2 "));
}

int main(int argc, char **argv) {
  testSetVar();
  testSimplestCheck();
  testSimplestFind();
  testSimplestFind2();
  testFileInput();
  testUf();
}