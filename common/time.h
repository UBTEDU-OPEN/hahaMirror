#ifndef TIME_H
#define TIME_H
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
std::string timeFormatStamp();
void updateNtpTime();

class CaculateFps
{
public:
    CaculateFps();
    ~CaculateFps() = default;

    std::string add();

private:
    int count_;
    int64_t start_time_;
};

class TimeConsumingAnalysis
{
public:
    TimeConsumingAnalysis()
        : lastpointname_("")
        , lastpointtime_(0)
        , index_(0)
    {
        addTimePoint();
    }
    ~TimeConsumingAnalysis() = default;

    void addTimePoint(std::string point);
    void addTimePoint();

    std::string print();
    void reset();

private:
    std::vector<std::pair<std::string, uint64_t>> points_;
    std::string lastpointname_;
    int64_t lastpointtime_;
    int index_;
};

} // namespace time
}; // namespace common

#endif
