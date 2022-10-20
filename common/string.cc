#include "string.h"

// using namespace common;
namespace common
{
std::string trimstr(std::string s)
{
    size_t n = s.find_last_not_of(" \r\n\t");
    if (n != std::string::npos)
    {
        s.erase(n + 1, s.size() - n);
    }
    n = s.find_first_not_of(" \r\n\t");
    if (n != std::string::npos)
    {
        s.erase(0, n);
    }
    return s;
}
} // namespace common
