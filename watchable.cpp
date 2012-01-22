#include <unistd.h>

#include "watchable.h"

void fd_watchable::close() {
  ::close(this->fd_);
}
