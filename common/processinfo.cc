#include "processinfo.h"
#include "muduo/base/ProcessInfo.h"

namespace common
{
namespace ProcessInfo
{
std::string exeDirPath()
{
    std::string exe_path = muduo::ProcessInfo::exePath();
    size_t pos = exe_path.rfind('/');
    if (pos != std::string::npos)
    {
        return exe_path.substr(0, pos);
    }

    return "";
}
} // namespace ProcessInfo
} // namespace common
