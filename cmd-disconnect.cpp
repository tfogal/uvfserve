#include <iostream>
#include <unistd.h>
#include "all-commands.h"
#include "watchable.h"

void disconnect(cmd_arg& arg) {
  if(close(arg.fd) != 0) {
    std::clog << "error closing fd: " << arg.fd << "\n";
  }
  throw watchable::remove(arg.fd);
}
