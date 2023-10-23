#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <optional>
import Sat;
import Sat2;
import Sat3;
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
  auto Output5 = Sat1.setLastVar(false);
  EXPECT_EQ(Output5->dumpStr(), "( ~x1 | x2 )");
}

void testFileInput() {
  auto [VarCount, Value] =
      inputFromFile(std::filesystem::path(__FILE__).parent_path() /
                    "cnf/for_tests/manual.cnf");
  Sat1_t SatManual(VarCount, std::move(Value));
  EXPECT_EQ(SatManual.dumpStr(), "( x1 | x2 | ~x3 ) & ( ~x1 | x2 )");
}

template <typename SatT> void testCheck() {
  SatT SatTrue(3, {{1, 2, -3}, {-1, 2}});
  EXPECT_EQ(SatTrue.check(), true);

  SatT SatFalse(1, {{1}, {-1}});
  EXPECT_EQ(SatFalse.check(), false);
}

// not for sat3
template <typename SatT> void testFind() {
  SatT SatFalse(1, {{1}, {-1}});
  EXPECT_EQ(SatFalse.find().has_value(), false);

  SatT Sat1(3, {{1, 2, -3}, {-1, 2}});
  auto Set1Str = Sat1.find();
  EXPECT_EQ(Set1Str.has_value(), true);
  EXPECT_EQ(*Set1Str, "~x1 ~x2 ~x3 ");

  SatT Sat2(3, {{1, 2, -3}, {1, 2}});
  EXPECT_EQ(*Sat2.find(), "x1 ~x2 ~x3 ");

  SatT Sat3(2, {{1, -2}, {1, 2}});
  EXPECT_EQ(*Sat3.find(), "x1 ~x2 ");

  SatT Sat4(2, {{1, 2}, {-1, 2}});
  EXPECT_EQ(*Sat4.find(), "~x1 x2 ");
}
void testFind3() { // to implement
  Sat3_t SatFalse(1, {{1}, {-1}});
  EXPECT_EQ(SatFalse.find().has_value(), false);

  Sat3_t Sat1(3, {{1, 2, -3}, {-1, 2}});
  auto Set1Str = Sat1.find();
  EXPECT_EQ(Set1Str.has_value(), true);
  EXPECT_EQ(*Set1Str, "x1 x2 ~x3 ");

  Sat3_t Sat2(3, {{1, 2, -3}, {1, 2}});
  EXPECT_EQ(*Sat2.find(), "x1 ~x2 ~x3 ");

  Sat3_t Sat3(2, {{1, -2}, {1, 2}});
  EXPECT_EQ(*Sat3.find(), "x1 ~x2 ");

  Sat3_t Sat4(2, {{1, 2}, {-1, 2}});
  EXPECT_EQ(*Sat4.find(), "x1 x2 ");
}

template <typename SatT> void testUf(std::string Path = "cnf/for_tests", std::string output_file = "") {
  std::vector<std::chrono::duration<double>> time_algo, time_algo2;
  std::vector<std::string> files;
  std::chrono::time_point<std::chrono::steady_clock> start, end;
  std::cout << Path << "/" << std::endl;
  for (const auto &entry : std::filesystem::directory_iterator(Path)) {
    std::cout << std::string(entry.path()) << std::endl;
    auto [VarCount, Value] = inputFromFile(entry.path());
    SatT SatTrue(VarCount, std::move(Value));
    EXPECT_EQ_MSG(SatTrue.check(), true,
                  std::string(entry.path()) +
                      " should be SAT, but Algo::simplestCheck says UNSAT");
    start = std::chrono::steady_clock::now();
    auto Set1 = SatTrue.find();
    end = std::chrono::steady_clock::now();
    time_algo.push_back(end - start);

    files.push_back(std::string(entry.path()));

    EXPECT_EQ_MSG(Set1.has_value(), true,
                  std::string(entry.path()) +
                      " should be SAT, but Algo::simplestFind says UNSAT");
  }
  /* this is time output */
  if (output_file != "") {
    std::fstream fs(output_file,std::fstream::trunc|std::fstream::out);
    if (fs.is_open()) {
      for (auto f:files)
        fs << f << " ";
      fs << std::endl;
      for (auto i:time_algo)
        fs << std::setprecision (3) << i.count() << " ";
      fs << std::endl;
    } else std::cout << "fs("+output_file+") isnt opened!\n";
    double s1=0, s2 = 0;
    for (auto i:time_algo)
      s1 += i.count();
    fs << "Mean Algo " << s1/time_algo.size() << std::endl;
  }
}

std::vector<std::string> getDirs(int argc, char** argv) {
  bool Flag = false;
  std::vector<std::string> SArgv;
  for (int i = 1; i < argc; i++)
    SArgv.push_back(std::string(argv[i]));
  std::vector<std::string> Dirs;
  for (auto s: SArgv) {
    if (s[0] == '-' && s != "-dir")
      Flag = false;
    if (Flag == true)
      Dirs.push_back(s);
    if (s == "-dir")
      Flag = true;
  }
  return Dirs;
}

int main(int argc, char** argv) {
  testForTest();
  testSetVar();
  testFileInput();
  testCheck<Sat1_t>();
  testFind<Sat1_t>();
  testCheck<Sat2_t>();
  testFind<Sat2_t>();
  testCheck<Sat3_t>();
  testFind3(); // different method to find final result
  auto Dirs = getDirs(argc, argv);
  for(auto d:Dirs){
    auto s = d;
    std::replace(s.begin(),s.end(),'/','_');
    testUf<Sat3_t>(d, "data/"+s+".data");
  }
  summary();
}