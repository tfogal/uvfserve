#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <iostream>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "all-commands.h"
#include "strfunc.h"

// synchronous send -- blocks/writes continually until all bytes are sent.
namespace {
  template<typename T> void ssendt(int fd, T data);
}
static void ssendarray(int fd, const uint8_t* data, size_t bytes);

// protocol: 32bit uint for filesize, file data
// file size of 0 indicates a bad filename.
void sendf(cmd_arg& arg) {
  std::string filename = trim(arg.arguments);
  int infd = open(filename.c_str(), O_RDONLY);
  if(infd < 0) {
    std::cerr << "Error opening '" << filename << "'\n";
    uint32_t sz = htonl(0); ssendt(arg.fd, sz); return;
  }
  struct stat buffer;
  if(stat(filename.c_str(), &buffer) != 0) {
    std::clog << "stat error: " << errno << "\n";
    uint32_t sz = htonl(0); ssendt(arg.fd, sz); return;
  }
  sendfile(arg.fd, infd, NULL, static_cast<size_t>(buffer.st_size));
  close(infd);
}

// synchronous send -- blocks/writes continually until all bytes are sent.
namespace {
  template<typename T> void ssendt(int fd, T data) {
    std::clog << "sending datum(" << fd << "): " << data << "\n";
    ssendarray(fd, reinterpret_cast<const uint8_t*>(&data), sizeof(data));
  }
}
static void ssendarray(int fd, const uint8_t* data, size_t bytes)
{
  size_t written = 0;
  do {
    ssize_t rv = write(fd, data+written, bytes-written);
    if(rv > 0) {
      written += rv;
    }
  } while(written < bytes && errno == EINTR);
}
