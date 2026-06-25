#ifndef UTILS_HPP
#define UTILS_HPP
#include <cstddef>
#include <iosfwd>
#include <string>
#include <utility>
namespace levkin {
  using Pair = std::pair< size_t, std::string >;
  struct Vec
  {
    size_t size;
    size_t cap;
    Pair* data;
  };
  Pair* reallocate(Vec& v);
  void readToVec(Vec& v, std::istream& is, size_t& total, size_t& ignored);
  void writeFromVec(const Vec& v, std::ostream& os);
}
#endif
