#ifndef UVFSERVER_COMMAND_H
#define UVFSERVER_COMMAND_H

#include <functional>
#include <string>
#include <vector>

// arguments to a command.
struct cmd_arg {
  std::vector<std::string>& filelist;
  int fd;
  std::string arguments;
  cmd_arg(std::vector<std::string>& fl, int f, std::string a):
    filelist(fl), fd(f), arguments(a) { }
};

struct command {
  std::string name;
  std::function<void(cmd_arg&)> func;
};

#endif /* UVFSERVER_COMMAND_H */
