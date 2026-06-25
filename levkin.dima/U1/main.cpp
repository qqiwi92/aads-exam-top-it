#include <iostream>
#include <fstream>
#include <string>
#include "utils.hpp"
int main(int argc, char* argv[])
{
  if (argc > 3) {
    std::cerr << "too many\n";
    return 0;
  }
  std::string in_file;
  std::string out_file;
  bool has_in = false;
  bool has_out = false;
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg.find("in:") == 0) {
      if (has_in) {
        return 1;
      }
      in_file = arg.substr(3);
      has_in = true;
    } else if (arg.find("out:") == 0) {
      if (has_out) {
        return 1;
      }
      out_file = arg.substr(4);
      has_out = true;
    } else {
      return 1;
    }
  }
  levkin::Vec< levkin::Pair > persons = {0, 0, nullptr};
  size_t total = 0;
  size_t ignored = 0;
  std::istream* input_stream = &std::cin;
  std::ifstream fileIs;
  if (has_in) {
    fileIs.open(in_file);
    if (!fileIs.is_open()) {
      return 2;
    }
    input_stream = &fileIs;
  }
  levkin::readToVec(persons, *input_stream, total, ignored);
  if (fileIs.is_open()) {
    fileIs.close();
  }
  std::ostream* output_stream = &std::cout;
  std::ofstream file_out;
  if (has_out) {
    file_out.open(out_file);
    if (!file_out.is_open()) {
      delete[] persons.data;
      return 2;
    }
    output_stream = &file_out;
  }
  levkin::writeFromVec(persons, *output_stream);
  if (file_out.is_open()) {
    file_out.close();
  }
  if (persons.data != nullptr) {
    delete[] persons.data;
  }

  if (total == 0) {
    ignored = 0;
    std::cout << "\n";
  }
  std::cerr << total << " " << ignored << "\n";
  return 0;
}
