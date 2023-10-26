#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <optional>

import Common;
import BaseSat;
import Sat1;
import Sat2;
import Sat3;
import Sat4;
import Sat5;
import Sat6;

auto FILEPATH = std::filesystem::path(__FILE__).parent_path();

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
  auto Output5 = Sat1.setMostImportantVar(false);
  EXPECT_EQ(Output5->dumpStr(), "( ~x1 | x2 )");
}

void testFileInput() {
  auto [VarCount, Value] = inputFromFile(FILEPATH / "cnf/manual.cnf");
  Sat1_t SatManual(VarCount, std::move(Value));
  EXPECT_EQ(SatManual.dumpStr(), "( x1 | x2 | ~x3 ) & ( ~x1 | x2 )");
}

template <typename SatT> void testCheck() {
  SatT SatTrue(3, {{1, 2, -3}, {-1, 2}});
  EXPECT_EQ(SatTrue.check(), true);

  SatT SatFalse(1, {{1}, {-1}});
  EXPECT_EQ(SatFalse.check(), false);

  auto [VarCount, Value] = inputFromFile(FILEPATH / "cnf/uf20-01.cnf");
  Sat1_t SatBig(VarCount, std::move(Value));
  EXPECT_EQ(SatBig.check(), true);
}

template <typename SatT> void testFind12() {
  SatT SatFalse(1, {{1}, {-1}});
  EXPECT_EQ(SatFalse.find().has_value(), false);

  SatT Sat1(3, {{1, 2, -3}, {-1, 2}});
  auto Set1Str = Sat1.find();
  EXPECT_EQ(Set1Str.has_value(), true);
  EXPECT_EQ(*Set1Str, "~x1 ~x2 ~x3");

  SatT Sat2(3, {{1, 2, -3}, {1, 2}});
  EXPECT_EQ(*Sat2.find(), "x1 ~x2 ~x3");

  SatT Sat3(2, {{1, -2}, {1, 2}});
  EXPECT_EQ(*Sat3.find(), "x1 ~x2");

  SatT Sat4(2, {{1, 2}, {-1, 2}});
  EXPECT_EQ(*Sat4.find(), "~x1 x2");
}

template <typename SatT = Sat3_t> void testFind3() {
  SatT SatFalse(1, {{1}, {-1}});
  EXPECT_EQ(SatFalse.find().has_value(), false);

  SatT Sat1(3, {{1, 2, -3}, {-1, 2}});
  auto Set1Str = Sat1.find();
  EXPECT_EQ(Set1Str.has_value(), true);
  EXPECT_EQ(*Set1Str, "x1 x2 ~x3");

  SatT Sat2(3, {{1, 2, -3}, {1, 2}});
  EXPECT_EQ(*Sat2.find(), "x1 ~x2 ~x3");

  SatT Sat3(2, {{1, -2}, {1, 2}});
  EXPECT_EQ(*Sat3.find(), "x1 ~x2");

  SatT Sat4(2, {{1, 2}, {-1, 2}});
  EXPECT_EQ(*Sat4.find(), "x1 x2");

  auto [VarCount, Value] = inputFromFile(FILEPATH / "cnf/uf20-01.cnf");
  SatT SatBig(VarCount, std::move(Value));
  EXPECT_EQ(*SatBig.find(), "~x1 x2 x3 x4 ~x5 ~x6 ~x7 x8 x9 x10 x11 ~x12 ~x13 "
                            "x14 x15 ~x16 x17 x18 x19 x20");
}

void testFind4() {
  Sat4_t SatFalse(1, {{1}, {-1}});
  EXPECT_EQ(SatFalse.find().has_value(), false);

  Sat4_t Sat1(3, {{1, 2, -3}, {-1, 2}});
  auto Set1Str = Sat1.find();
  EXPECT_EQ(Set1Str.has_value(), true);
  EXPECT_EQ(*Set1Str, "~x1 x2 ~x3");

  Sat4_t Sat2(3, {{1, 2, -3}, {1, 2}});
  EXPECT_EQ(*Sat2.find(), "x1 ~x2 ~x3");

  Sat4_t Sat3(2, {{1, -2}, {1, 2}});
  EXPECT_EQ(*Sat3.find(), "x1 ~x2");

  Sat4_t Sat4(2, {{1, 2}, {-1, 2}});
  EXPECT_EQ(*Sat4.find(), "x1 x2");

  auto [VarCount, Value] = inputFromFile(FILEPATH / "cnf/uf20-01.cnf");
  Sat4_t SatBig(VarCount, std::move(Value));
  EXPECT_EQ(*SatBig.find(), "~x1 x2 x3 x4 ~x5 ~x6 ~x7 x8 x9 x10 x11 ~x12 ~x13 "
                            "x14 x15 ~x16 x17 x18 x19 x20");
}

template <typename SatT = Sat5_t> void testFind5() {
  SatT SatFalse(1, {{1}, {-1}});
  EXPECT_EQ(SatFalse.find().has_value(), false);

  SatT Sat1(3, {{1, 2, -3}, {-1, 2}});
  auto Set1Str = Sat1.find();
  EXPECT_EQ(Set1Str.has_value(), true);
  EXPECT_EQ(*Set1Str, "x1 x2 ~x3");

  SatT Sat2(3, {{1, 2, -3}, {1, 2}});
  EXPECT_EQ(*Sat2.find(), "x1 ~x2 ~x3");

  SatT Sat3(2, {{1, -2}, {1, 2}});
  EXPECT_EQ(*Sat3.find(), "x1 ~x2");

  SatT Sat4(2, {{1, 2}, {-1, 2}});
  EXPECT_EQ(*Sat4.find(), "x1 x2");

  auto [VarCount, Value] = inputFromFile(FILEPATH / "cnf/uf20-01.cnf");
  SatT SatBig(VarCount, std::move(Value));
  EXPECT_EQ(*SatBig.find(), "x1 ~x2 ~x3 x4 ~x5 x6 ~x7 ~x8 ~x9 ~x10 ~x11 ~x12 "
                            "x13 x14 x15 ~x16 x17 ~x18 ~x19 x20");
}

template <typename SatT = Sat6_t> void testFind6() {
  SatT SatFalse(1, {{1}, {-1}});
  EXPECT_EQ(SatFalse.find().has_value(), false);

  SatT Sat1(3, {{1, 2, -3}, {-1, 2}});
  auto Set1Str = Sat1.find();
  EXPECT_EQ(Set1Str.has_value(), true);
  EXPECT_EQ(*Set1Str, "x1 x2 ~x3");

  SatT Sat2(3, {{1, 2, -3}, {1, 2}});
  EXPECT_EQ(*Sat2.find(), "x1 ~x2 ~x3");

  SatT Sat3(2, {{1, -2}, {1, 2}});
  EXPECT_EQ(*Sat3.find(), "x1 ~x2");

  SatT Sat4(2, {{1, 2}, {-1, 2}});
  EXPECT_EQ(*Sat4.find(), "x1 x2");

  auto [VarCount, Value] = inputFromFile(FILEPATH / "cnf/uf20-01.cnf");
  SatT SatBig(VarCount, std::move(Value));
  EXPECT_EQ(*SatBig.find(), "x1 ~x2 ~x3 x4 ~x5 ~x6 ~x7 ~x8 ~x9 x10 ~x11 ~x12 "
                            "x13 x14 x15 ~x16 x17 ~x18 ~x19 x20");
}

void testAnalyze4() {
  Sat4_t Sat1(4, {{1, 2, -3}, {4}, {-1, 2}});
  EXPECT_EQ(Sat1.dumpStr(), "( x4 ) & ( ~x1 | x2 ) & ( x1 | x2 | ~x3 )");
}
void testAnalyze5() {
  Sat5_t Sat1(2, {{1, 2}});
  EXPECT_EQ(Sat1.getMostImportantVar(), 1);
  Sat5_t Sat2(2, {{1, 2}, {-2}});
  EXPECT_EQ(Sat2.getMostImportantVar(), 2);
  Sat5_t Sat3(2, {{1, -2}, {-2}});
  EXPECT_EQ(Sat3.getMostImportantVar(), -2);
}

int main(int argc, char **argv) {
  testForTest();
  testSetVar();
  testFileInput();
  testAnalyze4();
  testAnalyze5();

  testCheck<Sat1_t>();
  testCheck<Sat2_t>();
  testCheck<Sat3_t>();
  testCheck<Sat4_t>();
  testCheck<Sat5_t>();
  testCheck<Sat6_t>();

  testFind12<Sat1_t>();
  testFind12<Sat2_t>();
  testFind3(); // different method to find final result
  testFind4();
  testFind5();
  testFind6();

  summary();
}