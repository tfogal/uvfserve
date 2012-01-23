#ifndef UVFSERVE_FS_SCAN_H
#define UVFSERVE_FS_SCAN_H

#include <string>
#include <vector>

std::vector<std::string> scan_for_uvfs(std::string directory);
bool is_uvf(std::string filename);

#endif /* UVFSERVE_FS_SCAN_H */
