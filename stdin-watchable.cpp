#include <iostream>
#include "stdin-watchable.h"
#include "shutdown.h"
#include "strfunc.h"

void stdin_watchable::mark_for_handling(uint32_t flags)
{
  this->bits = static_cast<short>(flags);
}

bool stdin_watchable::handle_event(std::vector<std::string>&)
{
  if(this->bits) {
    std::string cmd;
    std::cin >> cmd;
    if(trim(cmd) == "quit") {
      set_terminate_flag(true);
    }
    this->bits = 0;
    return true;
  }
  return false;
}
