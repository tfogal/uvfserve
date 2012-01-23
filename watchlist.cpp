#include <algorithm>
#include <iostream>
#include <memory>
#include <sys/inotify.h>
#include <sys/stat.h>
#include "watchlist.h"
#include "fs-scan.h"
#include "nonstd.h"
#include "watchable.h"

static bool exists(std::string filename)
{
  struct stat buffer;
  if(stat(filename.c_str(), &buffer) != 0) {
    if(errno == ENOENT) { return false; }
    std::clog << "stat error: " << errno << "\n";
    return false;
  }
  return true;
}

static std::string find_file(std::string file,
                             const std::vector<std::string>& dirs)
{
  for(auto i=dirs.begin(); i != dirs.end(); ++i) {
    std::string combined = *i + file;
    if(exists(combined)) {
      return combined;
    } else {
      std::clog << "'" << combined << "' does not exist.\n";
    }
  }
  return "";
}

// a special watchable which is only meant for cleaning up the root
// "inotify_init" watch descriptor.  As such, it never does anything, and never
// has any events.
class root_in_watchable : public fd_watchable {
  public:
    // needs the watch descriptor and a list of directories these files might
    // be in.
    root_in_watchable(int wd, std::vector<std::string> d):
      fd_watchable(wd), flags(0), dirs(d) { }
    virtual void mark_for_handling(uint32_t f) {
      this->flags = f;
    }
    virtual bool handle_event(std::vector<std::string>& filelist) {
      if(this->flags == 0) { return false; }

      // just using a base struct inotify_event isn't good enough, I guess
      // because the 'name' field is variably sized.  So just give it a giant
      // f-ing buffer and hopefully we won't hit that issue.
      size_t event_len = sizeof(struct inotify_event) + 4096;
      std::shared_ptr<struct inotify_event> event(
        static_cast<struct inotify_event*>(malloc(event_len)),
        nonstd::malloc_deleter<struct inotify_event>
      );
      std::clog << "Reading event from fd " << this->fd() << "\n";
      ssize_t bytes = read(this->fd(), event.get(), event_len);
      this->flags = 0;
      if(bytes <= 0) {
        std::cerr << "Error reading/processing an inotify event, errno="
                  << errno << "\n";
        return true;
      }
      // if we got here, we have popped off an event. the event might
      // be invalid, but we should always return true now because we
      // have modified the state.

      std::clog << "New event on WD " << event->wd << "\n";
      if(event->len == 0) {
        std::clog << "0 length name?  ignoring this event...\n";
        return true;
      }

      std::string filename = find_file(event->name, dirs);
      std::clog << "'" << event->name << "' -> '" << filename << "'\n";
      if(event->mask & IN_CLOSE_WRITE) {
        if(is_uvf(filename)) {
          std::clog << "adding new file: '" << filename << "'\n";
          filelist.push_back(filename);
        } else {
          std::clog << "new file '" << filename << "' is not a uvf.\n";
        }
      } else {
        std::clog << "removing file '" << event->name << "'\n";
        auto end = std::remove(filelist.begin(), filelist.end(), filename);
        filelist.erase(end, filelist.end());
      }

      return true;
    }

  private:
    uint32_t flags;
    std::vector<std::string> dirs; // where files may live
};

// faux class that really only exists to do cleanup of watch descriptors (i.e.
// to perform the inotify_rm_watch)
class inotify_watchable : public fd_watchable {
  public:
    inotify_watchable(int rwd, int wd) : fd_watchable(wd), rootwd(rwd) {}
    virtual ~inotify_watchable() {
      if(inotify_rm_watch(rootwd, this->fd()) != 0) {
        std::clog << "Error deleting watch " << this->fd() << ": " << errno
                  << "\n";
      }
    }
    void mark_for_handling(uint32_t f) {
      std::clog << "WD " << this->fd() << " has event: " << f << "\n";
    }
    bool handle_event(std::vector<std::string>&) { return false; }
  private:
    int rootwd;
};

watchlist initialize_watches(std::vector<std::string> dirs)
{
  watchlist wl;
  int in = inotify_init();
  for(auto i = dirs.begin(); i != dirs.end(); ++i) {
    int watch = inotify_add_watch(in, i->c_str(), IN_CLOSE_WRITE | IN_DELETE |
                                                  IN_ONLYDIR);
    std::clog << "new watch descriptor " << watch << " for directory "
              << *i << "\n";
    wl.push_back(std::shared_ptr<watchable>(new inotify_watchable(in, watch)));
  }

  wl.push_back(std::shared_ptr<watchable>(new root_in_watchable(in, dirs)));

  return wl;
}
