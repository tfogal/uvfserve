#include <arpa/inet.h>
#include <errno.h>
#include <inttypes.h>
#include <iostream>
#include "all-commands.h"

// synchronous send -- blocks/writes continually until all bytes are sent.
namespace {
  template<typename T> void ssendt(int fd, T data);
}
static void ssendarray(int fd, const uint8_t* data, size_t bytes);
static void send_string(std::string s, int fd)
{
  uint32_t sz = htonl(static_cast<uint32_t>(s.length()));
  ssendt(fd, sz);
  ssendarray(fd, reinterpret_cast<const uint8_t*>(s.c_str()), s.length());
}

// write a list of the file names to the client fd
// protocol is: 32bit-uint for the number of files, followed by n strings.
void list(cmd_arg& arg) {
  uint32_t sz = htonl(static_cast<uint32_t>(arg.filelist.size()));
  ssendt(arg.fd, sz);
  typedef std::vector<std::string> los;
  for(los::const_iterator f = arg.filelist.begin(); f != arg.filelist.end();
      ++f) {
    send_string(*f, arg.fd);
  }
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
