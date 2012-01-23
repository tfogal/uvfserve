#ifndef UVFSERVER_SOCKET_WATCHABLE_H
#define UVFSERVER_SOCKET_WATCHABLE_H

#include "watchable.h"

class socket_watchable : public fd_watchable {
  public:
    socket_watchable(int fd);

    virtual void mark_for_handling(uint32_t flags);
    virtual bool handle_event(std::vector<std::string>& filelist);

  protected:
    short bits;
};

class listen_socket_watchable: public socket_watchable {
  public:
    listen_socket_watchable(int fd) : socket_watchable(fd) { }
    virtual bool handle_event(std::vector<std::string>& filelist);
};

#endif /* UVFSERVER_SOCKET_WATCHABLE_H */
