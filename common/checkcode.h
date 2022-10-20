#ifndef COMMON_MD5_H
#define COMMON_MD5_H
#include <stddef.h>
#include <stdint.h>
#include <string>
#include <vector>

namespace common
{
namespace checkcode
{
std::string md5(const uint8_t* data, size_t size);
std::string md5(const std::vector<uint8_t> data);
std::string md5(const std::string&& data);
std::string md5(const std::string data);
} // namespace checkcode

} // namespace common

#endif // COMMON_MD5_H
