#include <iostream>
#include <sstream>
#include <unistd.h>
#include "watchable.h"
#include "strfunc.h"

void fd_watchable::close() {
  ::close(this->fd_);
}

cmd_watchable::cmd_watchable() { }
cmd_watchable::cmd_watchable(std::vector<command> l) : commands(l) { }
void cmd_watchable::set_command_list(std::vector<command> l) {
  commands = l;
}
void cmd_watchable::execute(std::string cline, cmd_arg& arg) {
  typedef std::vector<command> clist;
  std::istringstream parsefirst(cline);
  std::string cmd;
  parsefirst >> cmd;

  for(clist::const_iterator c = this->commands.begin();
      c != this->commands.end(); ++c) {
    if(c->name == cmd) {
      std::clog << "match!  executing '" << c->name << "'\n";
      if(cline.length() > cmd.length()) {
        arg.arguments = trim(cline.substr(cmd.length()+1, cline.length()));
      } else {
        arg.arguments = "";
      }
      c->func(arg);
    }
  }
}
