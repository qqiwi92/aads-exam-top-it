#ifndef DB_HPP
#define DB_HPP
#include <cstddef>
#include "utils.hpp"
namespace levkin {
  struct Meeting
  {
    size_t id1;
    size_t id2;
    size_t duration;
  };
  struct DB
  {
    Vec< levkin::Pair > persons;
    Vec< Meeting > meetings;
  };
  void freeDB(DB& db);
  bool hasPerson(const DB& db, size_t id);
  bool hasDescription(const DB& db, size_t id);
  void ensurePersonExists(DB& db, size_t id);
  int loadMeetings(DB& db, const std::string& data_file);
  void initDB(DB& db);
}
#endif
