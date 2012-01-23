#ifndef UVFSERVE_NONSTD_H
#define UVFSERVE_NONSTD_H

#include <cstdlib>

namespace nonstd {
  template<typename T> void array_deleter(T* t) { delete[] t; }

  template<typename T> void malloc_deleter(T* t) { free(t); }
}

#endif /* UVFSERVE_NONSTD_H */
