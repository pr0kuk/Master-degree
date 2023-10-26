#include <boost/program_options.hpp>
#include <cassert>
#include <chrono>
#include <filesystem>
#include <fstream>
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
constexpr int COUNT_SAT = 6;

namespace po = boost::program_options;
namespace fs = std::filesystem;
namespace ch = std::chrono;

struct Opt_t {
  std::optional<bool> Check;
  std::optional<int> RunSpecific;
  std::optional<fs::path> TestPathOpt;
  std::optional<fs::path> OutPathOpt;
};

std::optional<Opt_t> parseArgs(int Argc, char *Argv[]);

struct Time_t {
  ch::microseconds AnalyzeTime;
  ch::microseconds CheckTime;
  ch::microseconds FindTime;
};

void printTime(Time_t Time, std::string Info = "Time",
               std::optional<fs::path> = std::nullopt);

Time_t bench(int Num, fs::path TestPath, std::optional<bool> Check);

int main(int Argc, char *Argv[]) {
  auto Opt = parseArgs(Argc, Argv);
  if (!Opt)
    return 0;

  std::cout << "Test to execute: "
            << fs::canonical(fs::absolute(*Opt->TestPathOpt)) << "\n";
  if (Opt->OutPathOpt)
    std::cout << "Dump output to "
              << fs::canonical(fs::absolute(*Opt->OutPathOpt)) << "\n";

  if (Opt->RunSpecific) {
    printTime(bench(*Opt->RunSpecific, *Opt->TestPathOpt, Opt->Check),
              std::to_string(*Opt->RunSpecific), Opt->OutPathOpt);
    return 0;
  }

  for (int i = 2; i < COUNT_SAT + 1; ++i)
    printTime(bench(i, *Opt->TestPathOpt, Opt->Check), std::to_string(i),
              Opt->OutPathOpt);

  return 0;
}

// ----------------------- Implementations -----------------------------

template <typename SatT>
Time_t _bench(fs::path TestPath, std::optional<bool> Check);

Time_t bench(int Num, fs::path TestPath, std::optional<bool> Check) {
  switch (Num) {
  case 1:
    return _bench<Sat1_t>(TestPath, Check);
  case 2:
    return _bench<Sat2_t>(TestPath, Check);
  case 3:
    return _bench<Sat3_t>(TestPath, Check);
  case 4:
    return _bench<Sat4_t>(TestPath, Check);
  case 5:
    return _bench<Sat5_t>(TestPath, Check);
  case 6:
    return _bench<Sat6_t>(TestPath, Check);
  }
  std::cerr << "Sat " << std::to_string(Num) << " wasn't implemented!\n";
  return Time_t{};
}

template <typename SatT>
Time_t _bench(fs::path TestPath, std::optional<bool> Check) {
  auto [VarCount, Value] = inputFromFile(TestPath);
  SatT SatVar(VarCount, std::move(Value));

  auto AnalyzeTime = ch::high_resolution_clock::now();
  SatVar.analyze();
  auto CheckTimeStart = ch::high_resolution_clock::now();
  bool Result = SatVar.check();
  auto CheckTimeEnd = ch::high_resolution_clock::now();

  if (Check && *Check != Result)
    std::cerr << "\033[1;31mResult is " << Result << " but Check is " << *Check
              << "!\033[0m\n";

  auto FindTimeStart = ch::high_resolution_clock::now();
  SatVar.find();
  auto FindTimeEnd = ch::high_resolution_clock::now();

  return Time_t{
      ch::duration_cast<ch::microseconds>(CheckTimeStart - AnalyzeTime),
      ch::duration_cast<ch::microseconds>(CheckTimeEnd - CheckTimeStart),
      ch::duration_cast<ch::microseconds>(FindTimeEnd - FindTimeStart)};
}

template <typename T>
std::optional<T> readOption(po::variables_map &Vm, std::string OptName) {
  return Vm.count(OptName) ? std::make_optional(Vm[OptName].as<T>())
                           : std::nullopt;
}

std::optional<Opt_t> parseArgs(int Argc, char *Argv[]) {
  // Declare the supported options.
  po::options_description Desc("Allowed options");
  Desc.add_options()("help,h", "Produce help message")(
      "test,t", po::value<fs::path>(),
      "Path to get test to run")("output,o", po::value<fs::path>(),
                                 "Path to print result. Default=stdout")(
      "check", po::value<bool>(), "Check for SAT/UNSAT, wait for bool value")(
      "run", po::value<int>(),
      "Run specific SAT realization, wait for int value");

  po::variables_map Vm;
  po::store(po::parse_command_line(Argc, Argv, Desc), Vm);
  po::notify(Vm);

  if (Vm.count("help")) {
    std::cout << Desc << "\n";
    return std::nullopt;
  }

  Opt_t Opt;

  Opt.TestPathOpt = readOption<fs::path>(Vm, "test");
  Opt.OutPathOpt = readOption<fs::path>(Vm, "output");
  Opt.RunSpecific = readOption<int>(Vm, "run");
  Opt.Check = readOption<bool>(Vm, "check");

  if (Opt.TestPathOpt && fs::exists(*Opt.TestPathOpt) &&
      (!Opt.OutPathOpt || fs::exists(*Opt.OutPathOpt)))
    return Opt;

  // Process errors

  if (!Opt.TestPathOpt)
    std::cerr << "\033[1;31mYou have to specify test file!\n"
              << Desc << "\033[0m\n";

  if (Opt.TestPathOpt && !fs::exists(*Opt.TestPathOpt))
    std::cerr << "\033[1;31mTest path " << fs::absolute(*Opt.TestPathOpt)
              << " is not correct path!\n\033[0m";

  if (Opt.OutPathOpt && !fs::exists(*Opt.OutPathOpt))
    std::cerr << "\033[1;31mOutput path " << fs::absolute(*Opt.OutPathOpt)
              << " is not correct path!\n\033[0m";

  return std::nullopt;
}

template <typename T>
concept printable = requires(T a, std::string b) {
  a << b;
};
template <printable T>
static void _printTimeFormat(Time_t Time, std::string Info, T &&Out) {
  Out << "Sat: " << Info << "\n";
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