#ifndef UVFSERVER_WATCHABLE_H
#define UVFSERVER_WATCHABLE_H

#include <memory>
#include <inttypes.h>

struct watchable {
  virtual ~watchable() { }
  virtual int fd() const = 0;
  virtual void mark_for_handling(uint32_t flags) = 0;
  virtual void handle_event() = 0;

  struct create {
    create(std::shared_ptr<struct watchable> w) : entry(w) { }
    std::shared_ptr<struct watchable> entry;
  };
  struct remove {
    remove(int f) : fd(f) { }
    int fd;
  };
};

class fd_watchable : public watchable {
  public:
    fd_watchable(int fd) : fd_(fd) { }

    virtual int fd() const { return this->fd_; }

  private:
    int fd_;
};

#endif /* UVFSERVER_WATCHABLE_H */
