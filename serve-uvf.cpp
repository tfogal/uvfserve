#include <algorithm>
#include <arpa/inet.h>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <memory>
#include <netdb.h>
#include <poll.h>
#include <signal.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <vector>

#include "nonstd.h"
#include "socket-watchable.h"
#include "shutdown.h"
#include "stdin-watchable.h"
#include "watchable.h"

std::vector<std::string> scan_for_uvfs(std::string directory);
typedef std::vector<std::shared_ptr<watchable> > watchlist;
watchlist initialize_watches(std::vector<std::string> dirs);
std::shared_ptr<watchable> initialize_server(size_t port);
void wait_for_event(const watchlist&);
void term(int);

struct fd_equiv : public std::binary_function<std::shared_ptr<watchable>,
                                              int, bool> {
  bool operator()(const std::shared_ptr<watchable> w, int fd) {
    return w->fd() == fd;
  }
};

int main(int argc, char* argv[])
{
  if(argc <= 1) {
    std::cerr << "Need directory[ies] to watch.\n";
    exit(EXIT_FAILURE);
  }

  // every argument is a directory which should be scanned for UVFs.
  std::vector<std::string> directories;
  for(int i=1; i < argc; ++i) {
    directories.push_back(std::string(argv[i]));
  }
  std::vector<std::string> files;
  for(std::vector<std::string>::const_iterator dir = directories.begin();
      dir != directories.end(); ++dir) {
    std::vector<std::string> uvfs = scan_for_uvfs(*dir);
    files.insert(files.end(), uvfs.begin(), uvfs.end());
  }

  // set up an event handler for killing it...
  signal(SIGTERM, term);
  signal(SIGINT, term);

  // now we have our initial file list.  set up some watches so we'll know if
  // something changes.
  watchlist info = initialize_watches(directories);

  // start up our network server
  {
    info.push_back(initialize_server(4029));
  }

  info.push_back(std::shared_ptr<watchable>(new stdin_watchable()));

  // finally, loop waiting for a file to change or a connection to occur.
  do {
    wait_for_event(info);

    bool handled;
    do {
      handled = false;
      try {
        for(watchlist::const_iterator ev = info.begin(); ev != info.end();
            ++ev) {
          handled = (*ev)->handle_event();
        }
      } catch(const watchable::create & nw) {
        std::clog << "adding new watchable entry " << nw.entry.get() << "\n";
        info.push_back(nw.entry);
        handled = true;
      } catch(const watchable::remove & w) {
        std::clog << "removing watchable w/ fd " << w.fd << "\n";
        using namespace std::placeholders;
        watchlist::iterator rm = std::remove_if(info.begin(), info.end(),
                                                std::bind(fd_equiv(),_1,w.fd));
        info.erase(rm, info.end());
        handled = true;
      }
    } while(handled);
  } while(!get_terminate_flag());

  std::clog << "Closing " << info.size() << " active FDs...\n";
  for(watchlist::const_iterator ev = info.begin(); ev != info.end(); ++ev) {
    (*ev)->close();
  }
  info.clear();

  return 0;
}

std::vector<std::string> scan_for_uvfs(std::string /*directory*/)
{
  std::vector<std::string> rv; return rv;
}

watchlist initialize_watches(std::vector<std::string> /*dirs*/)
{
  watchlist retval;
  // FIXME implement!!!!
  return retval;
}
std::shared_ptr<watchable> initialize_server(size_t port)
{
  int sck = socket(AF_INET, SOCK_STREAM|SOCK_CLOEXEC, 0);
  if(sck == -1) {
    std::cerr << "could not create socket (errno=" << errno << ")\n";
    abort();
  }
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(struct sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;
  if(bind(sck, (const sockaddr*)&addr, sizeof(addr)) != 0) {
    std::cerr << "Could not bind socket (errno=" << errno << ")\n";
    close(sck);
    abort();
  }

  if(listen(sck, 5) != 0) {
    std::cerr << "Could set listen socket (errno=" << errno << ")\n";
    close(sck);
    abort();
  }

  return std::shared_ptr<watchable>(new listen_socket_watchable(sck));
}

void wait_for_event(const watchlist& watches)
{
  std::shared_ptr<struct pollfd> fds(new struct pollfd[watches.size()],
                                          nonstd::array_deleter<struct pollfd>);
  for(size_t i=0; i < watches.size(); ++i) {
    fds.get()[i].fd = watches[i]->fd();
    fds.get()[i].events = POLLIN | POLLPRI;
    fds.get()[i].revents = 0;
  }
  int revents = poll(fds.get(), watches.size(), -1);
  std::clog << revents << " events occurred.\n";
  if(revents == -1) {
    std::cerr << "poll error: " << errno << "\n";
    return;
  }
  assert(revents > 0);
  struct pollfd* begin = fds.get();
  struct pollfd* end = fds.get() + watches.size();
  size_t i=0;
  for(struct pollfd* cur = begin; cur != end; ++cur, ++i) {
    if(cur->revents & POLLNVAL) {
      throw "invalid file descriptor...\n";
    }
    if(cur->revents & POLLIN || cur->revents & POLLPRI ||
       cur->revents & POLLERR || cur->revents & POLLHUP) {
      watches[i]->mark_for_handling(cur->revents);
    }
  }
}

void term(int)
{
  set_terminate_flag(true);
}
