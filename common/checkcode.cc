#include "checkcode.h"

#include "md5.hpp"

namespace common
{
namespace checkcode
{
std::string md5(const uint8_t* data, size_t size)
{
    using namespace websocketpp::md5;

    unsigned char md5_result[17] = {0};
    md5_state_t state;
    md5_init(&state);
    md5_append(&state, data, size);
    md5_finish(&state, (uint8_t*) md5_result);

    std::string ret;
    ret.resize(32);
    sprintf(&ret[0],
            "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
            md5_result[0],
            md5_result[1],
            md5_result[2],
            md5_result[3],
            md5_result[4],
            md5_result[5],
            md5_result[6],
            md5_result[7],
            md5_result[8],
            md5_result[9],
            md5_result[10],
            md5_result[11],
            md5_result[12],
            md5_result[13],
            md5_result[14],
            md5_result[15]);

    return ret;
}

std::string md5(const std::vector<uint8_t> data)
{
    return md5(&data[0], data.size());
}

std::string md5(const std::string&& data)
{
    return md5((uint8_t*) &data[0], data.size());
}

std::string md5(const std::string data)
{
    return md5((uint8_t*) &data[0], data.size());
}

} // namespace checkcode

} // namespace common
