#ifndef COMMON_TIME_H
#define COMMON_TIME_H
#include <inttypes.h>
#include <string>

namespace common
{
namespace time
{
uint32_t getHDTimer();
uint64_t getCurrentTime();
uint64_t getCurrentMilliTime();
std::string timeFormatYMD();
std::string timeFormatString();
void updateNtpTime();

} // namespace time
}; // namespace common

#endif // COMMON_TIME_H
