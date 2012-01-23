#include <dirent.h>
#include <fstream>
#include <memory>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include "fs-scan.h"

std::vector<std::string> scan_for_uvfs(std::string directory)
{
  size_t len = offsetof(struct dirent, d_name) +
               pathconf(directory.c_str(), _PC_NAME_MAX) + 1;
  std::shared_ptr<dirent> ent(new dirent[len]);

  DIR* dir = opendir(directory.c_str());
  if(dir == NULL) {
    std::clog << "cannot open directory '" << directory << "': '" << errno
              << "\n";
    return std::vector<std::string>();
  }
  dirent* result;

  std::vector<std::string> uvfs;
  while((readdir_r(dir, ent.get(), &result)) == 0 && result) {
    std::clog << "testing '" << ent->d_name << "'\n";
    if(is_uvf(directory + std::string(ent->d_name))) {
      uvfs.push_back(directory + std::string(ent->d_name));
    }
  }
  std::clog << "found " << uvfs.size() << " UVFs.\n";
  closedir(dir);
  return uvfs;
}

static void first_block(const std::string& filename,
                        std::vector<int8_t>& block)
{
  std::ifstream ifs(filename.c_str(), std::ifstream::in |
                                      std::ifstream::binary);
  block.resize(8, 0);
  if(!ifs.is_open()) {
    std::clog << "could not open " << filename << "\n";
    return;
  }
  ifs.read(reinterpret_cast<char*>(&block[0]), 8);
  if(ifs.fail()) { block.resize(0); std::clog << "fail!\n"; }
  ifs.close();
}

bool is_uvf(std::string filename)
{
  std::vector<int8_t> magic;
  first_block(filename, magic);
  if(magic.size() == 0) { return false; } // read failed.

  return magic[0] == 'U' &&
         magic[1] == 'V' &&
         magic[2] == 'F' &&
         magic[3] == '-' &&
         magic[4] == 'D' &&
         magic[5] == 'A' &&
         magic[6] == 'T' &&
         magic[7] == 'A';
}
