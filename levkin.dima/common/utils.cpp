#include "utils.hpp"
#include <algorithm>

namespace levkin {
  void selectionSort(Vec< size_t >& vec)
  {
    for (size_t i = 0; i < vec.size; ++i) {
      for (size_t j = i + 1; j < vec.size; ++j) {
        if (vec.data[i] > vec.data[j]) {
          std::swap(vec.data[i], vec.data[j]);
        }
      }
    }
  }
}
