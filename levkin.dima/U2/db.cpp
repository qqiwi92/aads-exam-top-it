#include "db.hpp"
#include "db.hpp"
#include <fstream>
namespace levkin {
  void freeDB(DB& db)
  {
    freeVec(db.persons);
    freeVec(db.meetings);
  }
  bool hasPerson(const DB& db, size_t id)
  {
    for (size_t i = 0; i < db.persons.size; ++i) {
      if (db.persons.data[i].first == id)
        return true;
    }
    return false;
  }
  bool hasDescription(const DB& db, size_t id)
  {
    for (size_t i = 0; i < db.persons.size; ++i) {
      if (db.persons.data[i].first == id) {
        return !db.persons.data[i].second.empty();
      }
    }
    return false;
  }
  void ensurePersonExists(DB& db, size_t id)
  {
    if (!hasPerson(db, id)) {
      Pair p = std::make_pair(id, "");
      pushBack(db.persons, p);
    }
  }

  int loadMeetings(DB& db, const std::string& data_file)
  {
    std::ifstream dfs(data_file);
    if (!dfs) {
      std::cerr << "Error: Cannot open data file " << data_file << "\n";
      return 2;
    }

    size_t id1, id2, duration;
    while (dfs >> id1 >> id2 >> duration) {
      if (id1 == id2) {
        continue;
      }
      ensurePersonExists(db, id1);
      ensurePersonExists(db, id2);
      Meeting m{id1, id2, duration};
      pushBack(db.meetings, m);
    }

    if (!dfs.eof() && dfs.fail()) {
      std::cerr << "bad data in meeting file.\n";
      return 3;
    }

    return 0;
  }
  void initDB(DB& db)
  {
    db.persons.size = 0;
    db.persons.cap = 0;
    db.persons.data = nullptr;

    db.meetings.size = 0;
    db.meetings.cap = 0;
    db.meetings.data = nullptr;
  }
}
