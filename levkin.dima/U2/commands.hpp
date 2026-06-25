#ifndef COMMANDS_HPP
#define COMMANDS_HPP
#include <iosfwd>
namespace levkin {
  class DB;
  void anons(const DB& db, std::ostream& os);
  void deanon(DB& db, std::istream& is, std::ostream& os);
  void redesc(DB& db, std::istream& is, std::ostream& os);
  void desc(const DB& db, std::istream& is, std::ostream& os);
  void meets(const DB& db, std::istream& is, std::ostream& os);
  void commons(const DB& db, std::istream& is, std::ostream& os);
  void less(const DB& db, std::istream& is, std::ostream& os);
  void greater(const DB& db, std::istream& is, std::ostream& os);
  void outPersons(const DB& db, std::istream& is, std::ostream& os);

  void runInteractiveLoop(DB& db, std::istream& is, std::ostream& os);
  bool parseArguments(int argc,
                      char* argv[],
                      std::string& data_file,
                      std::string& in_file);
}
#endif
