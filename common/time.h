#ifndef COMMON_TIME_H
#define COMMON_TIME_H
#include <inttypes.h>
#include <string>
#include <vector>

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

class TimeConsumingAnalysis
{
public:
    TimeConsumingAnalysis()
        : lastpointname_("")
        , lastpointtime_(0)
        , index_(0)
    {
    }
    ~TimeConsumingAnalysis() = default;

    void addTimePoint(std::string point);
    void addTimePoint();

    std::string print();

private:
    void reset();

    std::vector<std::pair<std::string, uint64_t>> points_;
    std::string lastpointname_;
    int64_t lastpointtime_;
    int index_;
};

} // namespace time
}; // namespace common

#endif // COMMON_TIME_H
