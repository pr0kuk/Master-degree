import Algo;
import Algo2;
import <filesystem>;
import <fstream>;
import <chrono>;
import <iomanip>;
import <numeric>;
template <class T>
concept equality_comparable = requires(T a, T b) {
  a == b;
};

static int number_of_tests;
static int number_of_passed;
static int number_of_failed;

template <equality_comparable T>
bool compare(T Lhs, T Rhs, size_t Line, const char Filename[],
             const char Functionname[]) {
  number_of_tests++;
  if (Lhs == Rhs) {
    number_of_passed++;
    // std::cout << "\033[1;32mPASSED!\033[0m\n";
    return true;
  }
  number_of_failed++;
  std::cout << "\033[1;31mTEST \"" << Functionname << "\" FAILED!\n";
  std::cout << "Where: " << Filename << ":" << Line << std::endl;
  std::cout << "\033[1;34mLhs: " << Lhs << "\nRhs: " << Rhs << "\033[0m"
            << std::endl;
  return false;
}
template <equality_comparable T>
bool compare(T Lhs, T Rhs, std::string ErrMessage, size_t Line,
             const char Filename[], const char Functionname[]) {
  number_of_tests++;
  if (Lhs == Rhs) {
    number_of_passed++;
    // std::cout << "\033[1;32mPASSED!\033[0m\n";
    return true;
  }
  number_of_failed++;
  std::cout << "\033[1;31mTEST \"" << Functionname << "\" FAILED!\n";
  std::cout << "Where: " << Filename << ":" << Line << std::endl;
  std::cout << "\033[1;34mLhs: " << Lhs << "\nRhs: " << Rhs << "\033[0m"
            << std::endl;
  std::cout << "\033[1;33m" + ErrMessage << "\033[0m" << std::endl;
  return false;
}

bool compare(bool Cmp, size_t Line, const char Filename[],
             const char Functionname[]) {
  number_of_tests++;
  if (Cmp) {
    number_of_passed++;
    // std::cout << "\033[1;32mPASSED!\033[0m\n";
    return true;
  }
  number_of_failed++;
  std::cout << "\033[1;31mTEST \"" << Functionname << "\" FAILED!\n";
  std::cout << "Where: " << Filename << ":" << Line << std::endl;
  return false;
}

#define EXPECT_EQ_MSG(lhs, rhs, ErrMessage)                                    \
  compare(lhs, rhs, ErrMessage, __LINE__, __FILE__, __FUNCTION__)
#define EXPECT_EQ(lhs, rhs) compare(lhs, rhs, __LINE__, __FILE__, __FUNCTION__)
#define EXPECT_SET(cmp) compare(cmp, __LINE__, __FILE__, __FUNCTION__)

void summary() {
  std::cout << "\n\033[1;34mSUMMARY:\n";
  if (number_of_failed > 0)
    std::cout << "\033[1;31m[" << number_of_failed << "/" << number_of_tests
              << "] TESTS FAILED\n";
  if (number_of_passed > 0)
    std::cout << "\033[1;32m[" << number_of_passed << "/" << number_of_tests
              << "] TESTS PASSED\n";
}

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
  Sat::Sat_t SatTrue = Sat::inputFromFile("cnf/for_tests/manual.cnf");
  Sat2::Sat_t SatTrue2 = Sat2::inputFromFile("cnf/for_tests/manual.cnf");
  EXPECT_EQ(SatTrue.dumpStr(), std::string("( x1 | x2 | ~x3 ) & ( ~x1 | x2 )"));
  EXPECT_EQ(Algo::simplestCheck(SatTrue, 3), true);
  EXPECT_EQ(SatTrue2.dumpStr(),
            std::string("( x1 | x2 | ~x3 ) & ( ~x1 | x2 )"));
  EXPECT_EQ(Algo2::simplestCheck(SatTrue2, 3), true);
}

void testUf(std::string Path = "cnf/for_tests", std::string output_file = "") {
  std::vector<std::chrono::duration<double>> time_algo, time_algo2;
  std::vector<std::string> files;
  std::chrono::time_point<std::chrono::steady_clock> start, end;
  std::cout << Path << "/" << std::endl;
  for (const auto &entry : std::filesystem::directory_iterator(Path)) {
    std::cout << std::string(entry.path()) << std::endl;
    Sat::Sat_t SatTrue = Sat::inputFromFile(entry.path());
    Sat2::Sat_t SatTrue2 = Sat2::inputFromFile(entry.path());
    EXPECT_EQ_MSG(SatTrue.dumpStr(), SatTrue2.dumpStr(),
                  std::string(entry.path()) + " input differs in Sat and Sat2");

    EXPECT_EQ_MSG(Algo::simplestCheck(SatTrue, SatTrue.getVarCount()), true,
                  std::string(entry.path()) +
                      " should be SAT, but Algo::simplestCheck says UNSAT");
    EXPECT_EQ_MSG(Algo2::simplestCheck(SatTrue2, SatTrue2.getVarCount()), true,
                  std::string(entry.path()) +
                      " should be SAT, but Algo2::simplestCheck says UNSAT");

    start = std::chrono::steady_clock::now();
    auto Set1 = Algo::simplestFind(SatTrue);
    end = std::chrono::steady_clock::now();
    time_algo.push_back(end - start);
    start = end;
    auto Set2 = Algo2::simplestFind(SatTrue2);
    end = std::chrono::steady_clock::now();
    time_algo2.push_back(end - start);
    files.push_back(std::string(entry.path()));

    EXPECT_EQ_MSG(Set1.has_value(), true,
                  std::string(entry.path()) +
                      " should be SAT, but Algo::simplestFind says UNSAT");
    EXPECT_EQ_MSG(Set2.has_value(), true,
                  std::string(entry.path()) +
                      " should be SAT, but Algo2::simplestFind says UNSAT");

    // SatTrue.dump();
    // SatTrue2.dump();
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
      for (auto i:time_algo2)
        fs << std::setprecision (3) << (i).count() << " ";
      fs << std::endl;
    } else std::cout << "fs("+output_file+") isnt opened!\n";
    double s1=0, s2 = 0;
    for (auto i:time_algo)
      s1 += i.count();
    for (auto i:time_algo2)
      s2 += i.count();
    fs << "Mean Algo " << s1/time_algo.size() << std::endl;
    fs << "Mean Algo2 " << s2/time_algo2.size() << std::endl;
  }
}

void testSimplestFind() { // to implement
  Sat::Sat_t SatFalse({{1}, {-1}}, 1);
  auto SetFalse = Algo::simplestFind(SatFalse);
  EXPECT_EQ(SetFalse.has_value(), false);

  Sat::Sat_t Sat1({{1, 2, -3}, {-1, 2}}, 3);
  auto Set1 = Algo::simplestFind(Sat1);
  EXPECT_EQ(Set1.has_value(), true);
  EXPECT_EQ(*Set1, std::string("~x1 ~x2 ~x3 "));

  Sat::Sat_t Sat2({{1, 2, -3}, {1, 2}}, 3);
  auto Set2 = Algo::simplestFind(Sat2);
  EXPECT_EQ(*Set2, std::string("x1 ~x2 ~x3 "));

  Sat::Sat_t Sat3({{1, -2}, {1, 2}}, 2);
  EXPECT_EQ(*Algo::simplestFind(Sat3), std::string("x1 ~x2 "));
  Sat::Sat_t Sat4({{1, 2}, {-1, 2}}, 2);
  EXPECT_EQ(*Algo::simplestFind(Sat4), std::string("~x1 x2 "));
}

void testSimplestFind2() { // to implement
  Sat2::Sat_t SatFalse({{1}, {-1}}, 1);
  auto SetFalse = Algo2::simplestFind(SatFalse);
  EXPECT_EQ(SetFalse.has_value(), false);

  Sat2::Sat_t Sat1({{1, 2, -3}, {-1, 2}}, 3);
  auto Set1 = Algo2::simplestFind(Sat1);
  EXPECT_EQ(Set1.has_value(), true);
  EXPECT_EQ(*Set1, std::string("~x1 ~x2 ~x3 "));

  Sat2::Sat_t Sat2({{1, 2, -3}, {1, 2}}, 3);
  auto Set2 = Algo2::simplestFind(Sat2);
  EXPECT_EQ(*Set2, std::string("x1 ~x2 ~x3 "));

  Sat2::Sat_t Sat3({{1, -2}, {1, 2}}, 2);
  EXPECT_EQ(*Algo2::simplestFind(Sat3), std::string("x1 ~x2 "));

  Sat2::Sat_t Sat4({{1, 2}, {-1, 2}}, 2);
  EXPECT_EQ(*Algo2::simplestFind(Sat4), std::string("~x1 x2 "));
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

int main(int argc, char **argv) {
  testSetVar();
  testSimplestCheck();
  testSimplestFind();
  testSimplestFind2();
  testFileInput();
  testUf();
  auto Dirs = getDirs(argc, argv);
  for(auto d:Dirs){
    auto s = d;
    std::replace(s.begin(),s.end(),'/','_');
    testUf(d, "data/"+s+".data");
  }
  summary();
}