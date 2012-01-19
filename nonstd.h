#ifndef UVFSERVE_NONSTD_H
#define UVFSERVE_NONSTD_H

namespace nonstd {
  template<typename T> void array_deleter(T* t) { delete[] t; }
}

#endif /* UVFSERVE_NONSTD_H */
