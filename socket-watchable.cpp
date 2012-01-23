#include <cerrno>
#include <cstring>
#include <iostream>
#include <iterator>
#include <memory>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "socket-watchable.h"
#include "all-commands.h"
#include "command.h"
#include "nonstd.h"
#include "strfunc.h"

socket_watchable::socket_watchable(int fd) : fd_watchable(fd), bits(0)
{
  std::vector<command> cmds(3);
  cmds[0].name = "quit";
  cmds[0].func = disconnect;
  cmds[1].name = "list";
  cmds[1].func = list;
  cmds[2].name = "sendfile";
  cmds[2].func = sendf;
  this->set_command_list(cmds);
}

void socket_watchable::mark_for_handling(uint32_t flags)
{
  this->bits = static_cast<short>(flags);
}

bool socket_watchable::handle_event(std::vector<std::string>& filelist)
{
  bool did_something = false;
  if(this->bits & POLLIN) {
    did_something = true;
    std::shared_ptr<int8_t> buf(new int8_t[8192],
                                nonstd::array_deleter<int8_t>);
    ssize_t bytes = read(this->fd(), buf.get(), 8192);
    if(bytes == 0) { // disconnect.
      throw watchable::remove(this->fd());
    }
    std::string cmd(buf.get(), buf.get()+bytes);
    cmd = trim(cmd);
    std::cout << "cmd: '" << cmd << "' (" << bytes << " bytes)\n";
    cmd_arg arg(filelist, this->fd(), cmd /* hack, execute will reset it. */);
    this->execute(cmd, arg);
  }
  if(this->bits & POLLPRI) {
    // this shouldn't happen.. we don't use priority data.
    std::cerr << "priority data on fd " << this->fd() << "?!\n";
  }
  if(this->bits & POLLERR) {
    std::cerr << "error on file descriptor " << this->fd() << "\n";
  }
  if(this->bits & POLLHUP) {
    std::cerr << "hup on " << this->fd() << "\n";
    did_something = true;
    throw watchable::remove(this->fd());
  }
  this->bits = 0;
  return did_something;
}

// similar to socket_watchable's implementation, except that POLLIN
// means we should just accept someone, and POLLHUP no longer makes
// sense.
bool listen_socket_watchable::handle_event(std::vector<std::string>&) {
  std::clog << "listen socket event handling...\n";
  if(this->bits & POLLIN) {
    struct sockaddr_in saddr;
    socklen_t len = sizeof(struct sockaddr_in);
    memset(&saddr, 0, sizeof(struct sockaddr_in));
    int desc = accept4(this->fd(), reinterpret_cast<struct sockaddr*>(&saddr),
                       &len, SOCK_CLOEXEC);
    std::clog << "accepted FD " << desc << " from listen " << this->fd()
              << "\n";
    if(desc == -1) {
      std::cerr << "error accepting from fd(" << this->fd() << "); errno="
                << errno << "\n";
    }
    this->bits = 0;
    throw create(std::shared_ptr<watchable>(new socket_watchable(desc)));
  }
  if(this->bits & POLLPRI) {
    // this shouldn't happen.. we don't use priority data.
    std::cerr << "listen priority data on fd " << this->fd() << "?!\n";
  }
  if(this->bits & POLLERR) {
    std::cerr << "listen error on file descriptor " << this->fd() << "\n";
  }
  if(this->bits & POLLHUP) {
    std::cerr << "listen hup on " << this->fd() << "\n";
  }
  this->bits = 0;
  return false;
}
