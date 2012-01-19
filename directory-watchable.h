#ifndef UVFSERVER_DIRECTORY_WATCHABLE_H
#define UVFSERVER_DIRECTORY_WATCHABLE_H

class directory_watchable : public watchable {
  public:
    directory_watchable(int fd) : fd_watchable(fd) { }

    virtual void mark_for_handling(uint32_t flags);
    virtual void handle_event();
};

#endif /* UVFSERVER_DIRECTORY_WATCHABLE_H */
