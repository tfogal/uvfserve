#include <iostream>
#include "stdin-watchable.h"
#include "shutdown.h"

static std::string trim(std::string s) {
  s.erase(s.find_last_not_of(" \n\r\t")+1);
  return s;
}

void stdin_watchable::mark_for_handling(uint32_t flags)
{
  this->bits = static_cast<short>(flags);
}

bool stdin_watchable::handle_event()
{
  if(this->bits) {
    std::string cmd;
    std::cin >> cmd;
    if(trim(cmd) == "quit") {
      set_terminate_flag(true);
    }
    return true;
  }
  return false;
}
