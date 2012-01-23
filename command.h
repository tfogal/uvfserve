#ifndef UVFSERVER_COMMAND_H
#define UVFSERVER_COMMAND_H

#include <functional>
#include <string>
#include <vector>

// arguments to a command.
struct cmd_arg {
  std::vector<std::string>& filelist;
  int fd;
  cmd_arg(std::vector<std::string>& fl, int f) : filelist(fl), fd(f) { }
};

struct command {
  std::string name;
  std::function<void(cmd_arg&)> func;
};

#endif /* UVFSERVER_COMMAND_H */
