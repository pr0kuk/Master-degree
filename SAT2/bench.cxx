#include <boost/program_options.hpp>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <numeric>
#include <optional>

import Common;
import Sat;
import Sat2;
import Sat3;

namespace po = boost::program_options;
namespace fs = std::filesystem;
namespace ch = std::chrono;

std::optional<fs::path> parseArgs(int Argc, char *Argv[]);

template <typename SatT>
std::pair<ch::microseconds, ch::microseconds> bench(fs::path TestPath);

void printTime(std::pair<ch::microseconds, ch::microseconds> Time,
               std::string Info = "Time") {
  std::cout << Info << "\n";
  std::cout << "Check:\t" << Time.first.count() << "ms\tFind:\t"
            << Time.second.count() << "ms\n";
}

#define RUN_TEST(S) printTime(bench<S>(TestPath), #S)

int main(int Argc, char *Argv[]) {
  auto TestPathOpt = parseArgs(Argc, Argv);
  if (!TestPathOpt.has_value())
    return 1;

  auto TestPath = TestPathOpt.value();
  std::cout << "Test to execute: " << fs::canonical(fs::absolute(TestPath))
            << "\n";

  RUN_TEST(Sat1_t);
  RUN_TEST(Sat2_t);
  RUN_TEST(Sat3_t);

  return 0;
}

// ----------------------- Implementations -----------------------------

template <typename SatT>
std::pair<ch::microseconds, ch::microseconds> bench(fs::path TestPath) {
  auto [VarCount, Value] = inputFromFile(TestPath);
  SatT SatVar(VarCount, std::move(Value));

  auto StartTime = ch::high_resolution_clock::now();
  SatVar.check();
  auto MiddleTime = ch::high_resolution_clock::now();
  SatVar.find();
  auto EndTime = ch::high_resolution_clock::now();

  return std::make_pair(
      ch::duration_cast<ch::microseconds>(MiddleTime - StartTime),
      ch::duration_cast<ch::microseconds>(EndTime - MiddleTime));
}

std::optional<fs::path> parseArgs(int Argc, char *Argv[]) {
  // Declare the supported options.
  po::options_description Desc("Allowed options");
  Desc.add_options()("help,h", "Produce help message")(
      "test,t", po::value<fs::path>(), "Select test path to run");

  po::variables_map Vm;
  po::store(po::parse_command_line(Argc, Argv, Desc), Vm);
  po::notify(Vm);

  if (Vm.count("help")) {
    std::cout << Desc << "\n";
    return std::nullopt;
  }

  if (Vm.count("test")) {
    auto TestPath = Vm["test"].as<fs::path>();
    if (fs::exists(TestPath))
      return std::make_optional(TestPath);
    std::cerr << "Test path " << fs::absolute(TestPath)
              << " is not correct path!\n";
    return std::nullopt;
  }

  std::cerr << "You have to specify test file!\n";
  std::cerr << "Run -h to help!\n";
  return std::nullopt;
}
