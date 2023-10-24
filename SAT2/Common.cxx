module;

#include <cassert>
#include <concepts>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

export module Common;

// returns VarCount and Value_t for Sat
export std::pair<int, std::vector<std::vector<int>>>
inputFromFile(std::filesystem::path FilePath) {

  // open file
  std::ifstream In(FilePath);
  assert(In.is_open() && "Filename must be set correct!");

  // skip comments
  std::string Line;
  while (std::getline(In, Line) && Line[0] == 'c') {
  }

  // read basic data about test
  std::string Tmp;
  int VarCount = 0, NumClauses = 0;
  std::istringstream Ist(Line);
  Ist >> Tmp >> Tmp >> VarCount >> NumClauses;

  // read test data
  std::vector<std::vector<int>> Data;

  for (int i = 0; i < NumClauses; ++i) {
    std::vector<int> Clause;

    // read untill 0
    int tmp = 0;
    do {
      In >> tmp;
      Clause.push_back(tmp);
    } while (tmp != 0);

    // final 0 doesn't include
    Data.emplace_back(Clause.begin(), Clause.end() - 1);
  }

  In.close();
  return std::make_pair(VarCount, std::move(Data));
}
