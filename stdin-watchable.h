#ifndef UVFSERVER_STDIN_WATCHABLE_H
#define UVFSERVER_STDIN_WATCHABLE_H

#include <unistd.h>
#include "watchable.h"

class stdin_watchable : public fd_watchable {
  public:
    stdin_watchable() : fd_watchable(STDIN_FILENO), bits(0) { }

    virtual void mark_for_handling(uint32_t flags);
    virtual bool handle_event();
  private:
    short bits;
};

#endif /* UVFSERVER_STDIN_WATCHABLE_H */
