#include <iostream>
#include <unistd.h>

#include "watchable.h"

void fd_watchable::close() {
  ::close(this->fd_);
}

cmd_watchable::cmd_watchable() { }
cmd_watchable::cmd_watchable(std::vector<command> l) : commands(l) { }
void cmd_watchable::set_command_list(std::vector<command> l) {
  commands = l;
}
void cmd_watchable::execute(std::string cmd, cmd_arg& arg) {
  typedef std::vector<command> clist;
  for(clist::const_iterator c = this->commands.begin();
      c != this->commands.end(); ++c) {
    if(c->name == cmd) {
      std::clog << "match!  executing '" << c->name << "'\n";
      c->func(arg);
    }
  }
}
