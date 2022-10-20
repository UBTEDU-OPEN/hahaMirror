#ifndef COMMON_FILEOPT_H
#define COMMON_FILEOPT_H

#include <string>

namespace common
{
namespace fileopt
{
void saveImage(std::string filename, const uint8_t *facedata, const size_t facedata_len);

std::string getFileContent(std::string filepath);
std::string getFileMd5Value(std::string filepath);

bool isDir(std::string dirpath);
bool isExsit(std::string filepath);
bool createDirectory(const std::string filepath);
bool removeEmptyDirectory(std::string dirpath);
bool removeNotEmptyDirectory(const char *dir);

} // namespace fileopt
} // namespace common

#endif // COMMON_FILEOPT_H
