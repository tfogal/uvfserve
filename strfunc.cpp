#include "strfunc.h"

std::string trim(std::string s) {
  s.erase(s.find_last_not_of(" \n\r\t")+1);
  return s;
}
