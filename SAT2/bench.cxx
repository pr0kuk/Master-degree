#include <boost/program_options.hpp>
#include <cassert>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <numeric>
#include <optional>
import Common;
import Sat1;
import Sat2;
import Sat3;
import Sat4;

namespace po = boost::program_options;
namespace fs = std::filesystem;
namespace ch = std::chrono;

std::pair<std::optional<fs::path>, std::optional<fs::path>>
parseArgs(int Argc, char *Argv[]);

struct Time_t {
  ch::microseconds AnalyzeTime;
  ch::microseconds CheckTime;
  ch::microseconds FindTime;
};

template <typename SatT> Time_t bench(fs::path TestPath);

void printTime(Time_t Time, std::string Info = "Time",
               std::optional<fs::path> = std::nullopt);

#define RUN_TEST(S, Test, Out) printTime(bench<S>(*Test), #S, Out)

int main(int Argc, char *Argv[]) {
  auto [TestPathOpt, OutPathOpt] = parseArgs(Argc, Argv);
  if (!TestPathOpt.has_value())
    return 1;

  std::cout << "Test to execute: " << fs::canonical(fs::absolute(*TestPathOpt))
            << "\n";
  if (OutPathOpt)
    std::cout << "Dump output to " << fs::canonical(fs::absolute(*OutPathOpt))
              << "\n";

  RUN_TEST(Sat2_t, TestPathOpt, OutPathOpt);
  RUN_TEST(Sat3_t, TestPathOpt, OutPathOpt);
  RUN_TEST(Sat4_t, TestPathOpt, OutPathOpt);

  return 0;
}

// ----------------------- Implementations -----------------------------

template <typename SatT> Time_t bench(fs::path TestPath) {
  auto [VarCount, Value] = inputFromFile(TestPath);
  SatT SatVar(VarCount, std::move(Value));

  auto AnalyzeTime = ch::high_resolution_clock::now();
  SatVar.analyze();
  auto StartTime = ch::high_resolution_clock::now();
  SatVar.check();
  auto MiddleTime = ch::high_resolution_clock::now();
  SatVar.find();
  auto EndTime = ch::high_resolution_clock::now();

  return Time_t{ch::duration_cast<ch::microseconds>(StartTime - AnalyzeTime),
                ch::duration_cast<ch::microseconds>(MiddleTime - StartTime),
                ch::duration_cast<ch::microseconds>(EndTime - MiddleTime)};
}

std::pair<std::optional<fs::path>, std::optional<fs::path>>
parseArgs(int Argc, char *Argv[]) {
  // Declare the supported options.
  po::options_description Desc("Allowed options");
  Desc.add_options()("help,h", "Produce help message")(
      "test,t", po::value<fs::path>(), "Select test path to run")(
      "output,o", po::value<fs::path>(),
      "Select data path to out result. If no specified to stdout");

  po::variables_map Vm;
  po::store(po::parse_command_line(Argc, Argv, Desc), Vm);
  po::notify(Vm);

  std::optional<fs::path> OutPath =
      Vm.count("output") ? std::make_optional(Vm["output"].as<fs::path>())
                         : std::nullopt;
  std::optional<fs::path> TestPath =
      Vm.count("test") ? std::make_optional(Vm["test"].as<fs::path>())
                       : std::nullopt;
  if (TestPath && fs::exists(*TestPath) && (!OutPath || fs::exists(*OutPath)))
    return std::make_pair(TestPath, OutPath);

  // Process errors
  if (Vm.count("help"))
    std::cout << Desc << "\n";

  if (!TestPath)
    std::cerr << "You have to specify test file!\n" << Desc << "\n";

  if (TestPath && !fs::exists(*TestPath))
    std::cerr << "Test path " << fs::absolute(*TestPath)
              << " is not correct path!\n";

  if (OutPath && !fs::exists(*OutPath))
    std::cerr << "Output path " << fs::absolute(*OutPath)
              << " is not correct path!\n";

  return std::make_pair(std::nullopt, std::nullopt);
}

template <typename T>
concept printable = requires(T a, std::string b) { a << b; };
template <printable T>
static void _printTimeFormat(Time_t Time, std::string Info, T &&Out) {
  Out << Info << "\n";
  Out << "Analyze:\t" << Time.AnalyzeTime.count() << "\t"
      << "Check:\t" << Time.CheckTime.count() << "\t"
      << "Find:\t" << Time.FindTime.count() << "\n";
}

void printTime(Time_t Time, std::string Info, std::optional<fs::path> OutPath) {
  if (!OutPath) {
    _printTimeFormat(Time, Info, std::cout);
    return;
  }

  std::fstream FOut(*OutPath, std::fstream::out | std::fstream::app);
  if (!FOut) {
    std::cout << "Can't open " << OutPath.value() << " file to write dump!\n";
    return;
  }

  _printTimeFormat(Time, Info, FOut);
  FOut.close();
}