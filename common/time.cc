#include "time.h"
#include "logging.h"
#include <sys/time.h>

namespace common
{
namespace time
{
uint32_t getHDTimer()
{
    struct timespec t;
    t.tv_sec = t.tv_nsec = 0;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * 1000L + t.tv_nsec / 1000000L;
}
uint64_t getCurrentNanoTime()
{
#if HAS_CLOCK_GETTIME
    struct timespec tim;
    clock_gettime(CLOCK_REALTIME, &tim);
    return static_cast<uint64_t>(tim.tv_sec) * 1000000000LL + tim.tv_nsec;
#else
    struct timeval timeofday;
    gettimeofday(&timeofday, NULL);
    return static_cast<uint64_t>(timeofday.tv_sec) * 1000000000LL
           + static_cast<uint64_t>(timeofday.tv_usec) * 1000LL;
#endif
}

uint64_t getCurrentTime()
{
#if HAS_CLOCK_GETTIME
    struct timespec tim;
    clock_gettime(CLOCK_REALTIME, &tim);
    return static_cast<uint64_t>(tim.tv_sec) * 1000000000LL + tim.tv_nsec;
#else
    struct timeval timeofday;
    gettimeofday(&timeofday, NULL);
    return static_cast<uint64_t>(timeofday.tv_sec);
#endif
}

uint64_t getCurrentMilliTime()
{
#if HAS_CLOCK_GETTIME
    struct timespec tim;
    clock_gettime(CLOCK_REALTIME, &tim);
    return static_cast<uint64_t>(tim.tv_sec) * 1000000000LL + tim.tv_nsec;
#else
    struct timeval timeofday;
    gettimeofday(&timeofday, NULL);
    return static_cast<uint64_t>(timeofday.tv_sec) * 1000LL
           + static_cast<uint64_t>(timeofday.tv_usec) / 1000LL;
#endif
}

std::string timeFormatYMD()
{
    time_t rawtime;
    struct tm *info;
    char buffer[80];
    ::time(&rawtime);
    info = localtime(&rawtime);
    strftime(buffer, 80, "%Y%m%d", info);

    return std::string(buffer);
}

std::string timeFormatString()
{
    time_t rawtime;
    struct tm *info;
    char buffer[80];
    ::time(&rawtime);
    info = localtime(&rawtime);
    strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", info);

    return std::string(buffer);
}

std::string timeFormatStamp()
{
    time_t rawtime;
    struct tm *info;
    char buffer[80];
    ::time(&rawtime);
    info = localtime(&rawtime);
    strftime(buffer, 80, "%Y%m%d%H%M%S", info);

    return std::string(buffer);
}

void TimeConsumingAnalysis::addTimePoint()
{
    std::string point_name = "t" + std::to_string(index_++);
    addTimePoint(point_name);
}

void TimeConsumingAnalysis::addTimePoint(std::string point)
{
    if (point == "")
    {
        LOG_ERROR("point can't be empty!!");
    }
    uint64_t now = getCurrentMilliTime();

    if (lastpointtime_ == 0 && lastpointname_ == "")
    {
        lastpointname_ = point;
        lastpointtime_ = now;
        return;
    }

    points_.push_back({point, now - lastpointtime_});

    lastpointname_ = point;
    lastpointtime_ = now;
}

void TimeConsumingAnalysis::reset()
{
    lastpointname_ = "";
    lastpointtime_ = 0;

    index_ = 0;
    points_.clear();

    addTimePoint();
}

std::string TimeConsumingAnalysis::print()
{
    std::string result = "";
    for (auto it = points_.begin(); it != points_.end(); ++it)
    {
        result += it->first + ": " + std::to_string(it->second) + ", ";
    }

    if (result != "")
    {
        int size = result.size();
        result.erase(size - 1);
        result.erase(size - 2);
    }

    reset();

    return result;
}

CaculateFps::CaculateFps()
    : count_(0)
    , start_time_(getCurrentMilliTime())
{
}

std::string CaculateFps::add()
{
    ++count_;
    if (getCurrentMilliTime() - start_time_ > 1000)
    {
        int diff_time = getCurrentMilliTime() - start_time_;
        int fps = count_ * 1000 / diff_time;
        count_ = 0;
        start_time_ = getCurrentMilliTime();
        return std::to_string(fps);
    }

    return "";
}

} // namespace time
} // namespace common
