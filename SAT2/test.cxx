#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>

import Sat;
import Common;

template <class T1, class T2>
concept equality_comparable = requires(T1 a, T2 b) {
  a == b;
};

static size_t NumPass = 0;
static size_t NumFail = 0;

std::string getBaseName(std::string Name) {
  auto Pos = Name.rfind('/') + 1;
  return Name.substr(Pos, Name.size() - Pos);
}

template <typename T1, equality_comparable<T1> T2>
bool compare(T1 Lhs, T2 Rhs, std::string LhsMsg, std::string RhsMsg,
             size_t Line, std::string Filename, const char Functionname[],
             bool IsTrueCmp) {
  auto BaseFilename = getBaseName(Filename);
  if ((Lhs == Rhs) ^ !IsTrueCmp) {
    ++NumPass;
    std::cout << "\033[1;32m" << BaseFilename << ":" << Line << "\t"
              << Functionname << "\tPASSED!\033[0m\n";
    return true;
  }

  ++NumFail;
  std::cout << "\033[1;31m" << BaseFilename << ":" << Line << "\t"
            << Functionname << "\tFAILED!\033[0m\n\033[1;35m";
  std::cout << "Left side:\t" << LhsMsg << ": " << Lhs << "\n";
  std::cout << "Right side:\t" << RhsMsg << ": " << Rhs << "\033[0m\n";
  return false;
}

void summary() {
  std::cout << "\n\033[1;34mSUMMARY:\n";
  if (NumFail > 0)
    std::cout << "\033[1;31m[" << NumFail << "/" << NumFail + NumPass
              << "] TESTS FAILED\n";
  if (NumPass > 0)
    std::cout << "\033[1;32m[" << NumPass << "/" << NumFail + NumPass
              << "] TESTS PASSED\n";
}

#define EXPECT_EQ(lhs, rhs)                                                    \
  compare(lhs, rhs, #lhs, #rhs, __LINE__, __FILE__, __FUNCTION__, true)
#define EXPECT_NEQ(lhs, rhs)                                                   \
  compare(lhs, rhs, #lhs, #rhs, __LINE__, __FILE__, __FUNCTION__, false)

void testForTest() {
  EXPECT_EQ(true, 1);
  EXPECT_NEQ(true, 0);
}

void testSetVar() {
  Sat1_t Sat1(3, {{1, 2, -3}, {-1, 2}});
  // ( x1 | x2 | ~x3 ) & ( ~x1 | x2 )

  auto Output1 = Sat1.setVar(-1);
  EXPECT_EQ(Output1->dumpStr(), "( x2 | ~x3 )");
  auto Output2 = Sat1.setVar(1);
  EXPECT_EQ(Output2->dumpStr(), "( x2 )");
  auto Output3 = Sat1.setVar(2);
  EXPECT_EQ(Output3->dumpStr(), "");
  auto Output4 = Sat1.setVar(3);
  EXPECT_EQ(Output4->dumpStr(), "( x1 | x2 ) & ( ~x1 | x2 )");
}

void testFileInput() {
  auto [VarCount, Value] = inputFromFile(std::filesystem::current_path() /
                                         "cnf/for_tests/manual.cnf");
  Sat1_t SatManual(VarCount, std::move(Value));
  EXPECT_EQ(SatManual.dumpStr(), "( x1 | x2 | ~x3 ) & ( ~x1 | x2 )");
}

int main() {
  testForTest();
  testSetVar();
  testFileInput();
  summary();
}