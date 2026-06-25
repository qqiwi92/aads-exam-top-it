#include <iostream>
#include <fstream>
#include <string>
#include "db.hpp"
#include "commands.hpp"
#include "utils.hpp"

int main(int argc, char* argv[])
{
  if (argc < 2 || argc > 3) {
    std::cerr << "Error: Invalid number of arguments.\n";
    return 1;
  }

  std::string data_file;
  std::string in_file;

  if (!levkin::parseArguments(argc, argv, data_file, in_file)) {
    return 1;
  }

  levkin::DB db;
  levkin::initDB(db);

  if (!in_file.empty()) {
    std::ifstream ifs(in_file);
    if (!ifs) {
      std::cerr << "Error: Cannot open input file " << in_file << "\n";
      return 2;
    }
    size_t total = 0, ignored = 0;
    levkin::readToVec(db.persons, ifs, total, ignored);
  }

  int load_status = levkin::loadMeetings(db, data_file);
  if (load_status != 0) {
    levkin::freeDB(db);
    return load_status;
  }

  levkin::runInteractiveLoop(db, std::cin, std::cout);

  levkin::freeDB(db);
  return 0;
}
