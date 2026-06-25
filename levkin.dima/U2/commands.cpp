#include "commands.hpp"
#include "utils.hpp"
#include "db.hpp"
#include <fstream>

namespace levkin {
  namespace {
    void findCommonElements(const Vec< size_t >& v1,
                            const Vec< size_t >& v2,
                            Vec< size_t >& res)
    {
      for (size_t i = 0; i < v1.size; ++i) {
        for (size_t j = 0; j < v2.size; ++j) {
          if (v1.data[i] == v2.data[j]) {
            pushBack(res, v1.data[i]);
            break;
          }
        }
      }
    }
    void collectUniquePartners(const DB& db,
                               size_t target_id,
                               Vec< size_t >& partners)
    {
      for (size_t i = 0; i < db.meetings.size; ++i) {
        const auto& m = db.meetings.data[i];
        if (m.id1 == target_id || m.id2 == target_id) {
          size_t partner = (m.id1 == target_id) ? m.id2 : m.id1;
          bool is_duplicate = false;
          for (size_t j = 0; j < partners.size; ++j) {
            if (partners.data[j] == partner) {
              is_duplicate = true;
              break;
            }
          }
          if (!is_duplicate) {
            pushBack(partners, partner);
          }
        }
      }
    }
    void sortMeetings(Vec< Meeting >& vec)
    {
      for (size_t i = 0; i < vec.size; ++i) {
        for (size_t j = i + 1; j < vec.size; ++j) {
          if (vec.data[i].id1 > vec.data[j].id1
              || (vec.data[i].id1 == vec.data[j].id1
                  && vec.data[i].duration > vec.data[j].duration)) {
            std::swap(vec.data[i], vec.data[j]);
          }
        }
      }
    }
    void printFilterMeets(const DB& db,
                          size_t id,
                          std::ostream& os,
                          int mode,
                          size_t limit_dur = 0)
    {
      Vec< Meeting > user_meets{0, 0, nullptr};
      for (size_t i = 0; i < db.meetings.size; ++i) {
        const auto& m = db.meetings.data[i];
        if (m.id1 == id || m.id2 == id) {
          size_t partner = (m.id1 == id) ? m.id2 : m.id1;
          if (mode == 1 && m.duration >= limit_dur)
            continue;
          if (mode == 2 && m.duration <= limit_dur)
            continue;
          Meeting nm{partner, id, m.duration};
          pushBack(user_meets, nm);
        }
      }
      sortMeetings(user_meets);
      for (size_t i = 0; i < user_meets.size; ++i) {
        os << user_meets.data[i].id1 << " " << user_meets.data[i].duration
           << "\n";
      }
      freeVec(user_meets);
    }
  }
  void anons(const DB& db, std::ostream& os)
  {
    Vec< size_t > anon_ids{0, 0, nullptr};
    for (size_t i = 0; i < db.persons.size; ++i) {
      if (db.persons.data[i].second.empty()) {
        pushBack(anon_ids, db.persons.data[i].first);
      }
    }
    selectionSort(anon_ids);
    for (size_t i = 0; i < anon_ids.size; ++i) {
      os << anon_ids.data[i] << "\n";
    }
    freeVec(anon_ids);
  }
  void desc(const DB& db, std::istream& is, std::ostream& os)
  {
    size_t id;
    if (!(is >> id) || !hasPerson(db, id)) {
      os << "<INVALID COMMAND>\n";
      is.clear();
      return;
    }
    for (size_t i = 0; i < db.persons.size; ++i) {
      if (db.persons.data[i].first == id) {
        if (db.persons.data[i].second.empty())
          os << "<ANON>\n";
        else
          os << db.persons.data[i].second << "\n";
        break;
      }
    }
  }
  void deanon(DB& db, std::istream& is, std::ostream& os)
  {
    size_t id_anon, id_desc;
    if (!(is >> id_anon >> id_desc) || !hasPerson(db, id_anon)
        || !hasPerson(db, id_desc) || hasDescription(db, id_anon)
        || !hasDescription(db, id_desc)) {
      os << "<INVALID COMMAND>\n";
      is.clear();
      return;
    }
    for (size_t i = 0; i < db.meetings.size; ++i) {
      if (db.meetings.data[i].id1 == id_anon)
        db.meetings.data[i].id1 = id_desc;
      if (db.meetings.data[i].id2 == id_anon)
        db.meetings.data[i].id2 = id_desc;
    }
    size_t write_idx = 0;
    for (size_t i = 0; i < db.meetings.size; ++i) {
      if (db.meetings.data[i].id1 != db.meetings.data[i].id2) {
        db.meetings.data[write_idx++] = db.meetings.data[i];
      }
    }
    db.meetings.size = write_idx;
    size_t p_write_idx = 0;
    for (size_t i = 0; i < db.persons.size; ++i) {
      if (db.persons.data[i].first != id_anon) {
        db.persons.data[p_write_idx++] = db.persons.data[i];
      }
    }
    db.persons.size = p_write_idx;
  }
  void redesc(DB& db, std::istream& is, std::ostream& os)
  {
    size_t id;
    if (!(is >> id) || !hasPerson(db, id)) {
      os << "<INVALID COMMAND>\n";
      is.clear();
      std::string dummy;
      std::getline(is, dummy);
      return;
    }
    char ch;
    while (is.get(ch) && isSpaceChar(ch))
      ;
    if (ch != '"') {
      os << "<INVALID COMMAND>\n";
      is.clear();
      std::string dummy;
      std::getline(is, dummy);
      return;
    }
    std::string new_desc = "";
    while (is.get(ch) && ch != '"') {
      new_desc += ch;
    }
    for (size_t i = 0; i < db.persons.size; ++i) {
      if (db.persons.data[i].first == id) {
        db.persons.data[i].second = new_desc;
        break;
      }
    }
  }
  void meets(const DB& db, std::istream& is, std::ostream& os)
  {
    size_t id;
    if (!(is >> id) || !hasPerson(db, id)) {
      os << "<INVALID COMMAND>\n";
      is.clear();
      return;
    }
    printFilterMeets(db, id, os, 0);
  }
  void less(const DB& db, std::istream& is, std::ostream& os)
  {
    size_t id, duration;
    if (!(is >> id >> duration) || !hasPerson(db, id)) {
      os << "<INVALID COMMAND>\n";
      is.clear();
      return;
    }
    printFilterMeets(db, id, os, 1, duration);
  }
  void greater(const DB& db, std::istream& is, std::ostream& os)
  {
    size_t id, duration;
    if (!(is >> id >> duration) || !hasPerson(db, id)) {
      os << "<INVALID COMMAND>\n";
      is.clear();
      return;
    }
    printFilterMeets(db, id, os, 2, duration);
  }
  void commons(const DB& db, std::istream& is, std::ostream& os)
  {
    size_t id1, id2;
    if (!(is >> id1 >> id2) || !hasPerson(db, id1) || !hasPerson(db, id2)) {
      os << "<INVALID COMMAND>\n";
      is.clear();
      return;
    }
    Vec< size_t > partners1 = Vec< size_t >();
    Vec< size_t > partners2 = Vec< size_t >();
    Vec< size_t > common_ids = Vec< size_t >();
    try {
      collectUniquePartners(db, id1, partners1);
      collectUniquePartners(db, id2, partners2);
      findCommonElements(partners1, partners2, common_ids);
      selectionSort(common_ids);
      for (size_t i = 0; i < common_ids.size; ++i) {
        os << common_ids.data[i] << "\n";
      }
      freeVec(partners1);
      freeVec(partners2);
      freeVec(common_ids);
    } catch (...) {
      freeVec(partners1);
      freeVec(partners2);
      freeVec(common_ids);
      throw;
    }
  }
  void outPersons(const DB& db, std::istream& is, std::ostream& os)
  {
    std::string filename;
    if (!(is >> filename)) {
      os << "<INVALID COMMAND>\n";
      is.clear();
      return;
    }

    std::ofstream ofs(filename);
    if (!ofs) {
      os << "<INVALID COMMAND>\n";
      return;
    }

    for (size_t i = 0; i < db.persons.size; ++i) {
      if (!db.persons.data[i].second.empty()) {
        ofs << db.persons.data[i].first << " " << db.persons.data[i].second
            << "\n";
      }
    }
  }
  void runInteractiveLoop(DB& db, std::istream& is, std::ostream& os)
  {
    std::string cmd;
    while (is >> cmd) {
      if (cmd == "anons") {
        anons(db, os);
      } else if (cmd == "deanon") {
        deanon(db, is, os);
      } else if (cmd == "redesc") {
        redesc(db, is, os);
      } else if (cmd == "desc") {
        desc(db, is, os);
      } else if (cmd == "meets") {
        meets(db, is, os);
      } else if (cmd == "commons") {
        commons(db, is, os);
      } else if (cmd == "less") {
        less(db, is, os);
      } else if (cmd == "greater") {
        greater(db, is, os);
      } else if (cmd == "out-persons") {
        outPersons(db, is, os);
      } else {
        os << "<INVALID COMMAND>\n";
        std::string dummy;
        std::getline(is, dummy);
      }
    }
  }
  bool parseArguments(int argc,
                      char* argv[],
                      std::string& data_file,
                      std::string& in_file)
  {
    data_file = "";
    in_file = "";

    for (int i = 1; i < argc; ++i) {
      std::string arg = argv[i];
      if (arg.rfind("data:", 0) == 0) {
        if (!data_file.empty()) {
          std::cerr << "Error: Duplicate data argument.\n";
          return false;
        }
        data_file = arg.substr(5);
      } else if (arg.rfind("in:", 0) == 0) {
        if (!in_file.empty()) {
          std::cerr << "Error: Duplicate in argument.\n";
          return false;
        }
        in_file = arg.substr(3);
      } else {
        std::cerr << "Error: Invalid argument format.\n";
        return false;
      }
    }

    if (data_file.empty()) {
      std::cerr << "Error: Missing data file argument.\n";
      return false;
    }

    return true;
  }
}
