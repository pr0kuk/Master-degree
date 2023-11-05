#include <array>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <optional>

import testData;
import Sat1;

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

template <typename T1, equality_comparable<T1> T2>
consteval void compare(T1 Lhs, T2 Rhs) {
  if (Lhs != Rhs)
    throw "Compile test failed!";
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
#define EXPECT_EQ_COMPILE(lhs, rhs) compare(lhs, rhs)

void testForTest() {
  EXPECT_EQ(true, 1);
  EXPECT_NEQ(true, 0);
}

#define USE_TEST(Class, Test)                                                  \
  Class<Test.getClauseCount()>(Test.getData(), Test.getVarCount())

consteval void testCompileConstant() {
  EXPECT_EQ_COMPILE(TestSmall_1.getVarCount(), 3);
  EXPECT_EQ_COMPILE(TestSmall_1.getClauseCount(), 1);
  EXPECT_EQ_COMPILE(TestSmall_1.getData(0, 0), 1);

  auto Sat1 = USE_TEST(Sat1_t, TestSmall_1);
  EXPECT_EQ_COMPILE(Sat1.check(), true);
}

consteval void testAnalyzeSat1() {
  auto Sat1 = USE_TEST(Sat1_t, TestSmall_1);
  EXPECT_EQ_COMPILE(Sat1.analyze(1), 0);
  EXPECT_EQ_COMPILE(Sat1.analyze(3), 0);
  EXPECT_EQ_COMPILE(Sat1.analyze(-1), 1);

  auto Sat2 = USE_TEST(Sat1_t, TestSmall_2);
  EXPECT_EQ_COMPILE(Sat2.analyze(-2), 2);
  EXPECT_EQ_COMPILE(Sat2.analyze(2), 1);
  EXPECT_EQ_COMPILE(Sat2.analyze(1), 1);
  EXPECT_EQ_COMPILE(Sat2.analyze(-1), -1);

  auto Sat3 = USE_TEST(Sat1_t, TestSmall_3);
  EXPECT_EQ_COMPILE(Sat3.analyze(1), 0);
}

consteval bool operator==(const std::array<int, 3> &Lhs,
                          const std::array<int, 3> &Rhs) {
  return std::equal(Lhs.begin(), Lhs.end(), Rhs.begin());
}

consteval void testSetVar1() {
  auto Sat1 = USE_TEST(Sat1_t, TestSmall_1);
  auto NewSat = Sat1.setVar(1);
  EXPECT_EQ_COMPILE(Sat1.getImportantClause(), 1);
  EXPECT_EQ_COMPILE(NewSat.getImportantClause(), 0);
  EXPECT_EQ_COMPILE(NewSat[0], (std::array<int, 3>({0, 0, 0})));
  EXPECT_EQ_COMPILE(Sat1.setVar(-1)[0], (std::array<int, 3>({0, 2, 3})));

  auto Sat2 = USE_TEST(Sat1_t, TestSmall_2);
  auto NewSat2 = Sat2.setVar(1);
  EXPECT_EQ_COMPILE(NewSat2.getImportantClause(), 1);
  EXPECT_EQ_COMPILE(NewSat2[0], (std::array<int, 3>({0, 0, 0})));
  EXPECT_EQ_COMPILE(NewSat2[1], (std::array<int, 3>({0, 2, 0})));
  auto NewSat21 = Sat2.setVar(2);
  EXPECT_EQ_COMPILE(NewSat21.getImportantClause(), 1);
  EXPECT_EQ_COMPILE(NewSat21[0], (std::array<int, 3>({1, 0, 0})));
  EXPECT_EQ_COMPILE(NewSat21[1], (std::array<int, 3>({0, 0, 0})));
  EXPECT_EQ_COMPILE(NewSat21.setVar(1).getState(), SAT_STATE::TRUE);
  EXPECT_EQ_COMPILE(NewSat21.setVar(-1).getState(), SAT_STATE::FALSE);

  auto Sat3 = USE_TEST(Sat1_t, TestSmall_3);
  EXPECT_EQ_COMPILE(Sat3.setVar(1).getImportantClause(), 0);
  EXPECT_EQ_COMPILE(Sat3.setVar(-1).getImportantClause(), -1);
}

consteval void testCheck1() {
  EXPECT_EQ_COMPILE(USE_TEST(Sat1_t, TestSmall_1).check(), true);
  EXPECT_EQ_COMPILE(USE_TEST(Sat1_t, TestSmall_2).check(), true);
  EXPECT_EQ_COMPILE(USE_TEST(Sat1_t, TestSmall_4).check(), false);
  // EXPECT_EQ_COMPILE(USE_TEST(Sat1_t, TestUF_20).check(), false);
}

int main(int argc, char **argv) {
  testForTest();
  testCompileConstant();
  testSetVar1();
  testCheck1();
  summary();
}