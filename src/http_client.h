#ifndef JUMP_HTTP_HTTPCLIENT_H
#define JUMP_HTTP_HTTPCLIENT_H

#include <curl/curl.h>
#include <string>

#include "common/json.hpp"

namespace jump
{
namespace http
{
typedef struct _HttpConfig
{
    std::string url;

    struct
    {
        std::string token = "oauth/2.0/token";
        std::string identify = "face/faceset/user/identify/v1";
    } record;

    struct
    {
        bool fixed = false;
        std::string id = "20220214";
    } group;

    struct
    {
        std::string grant_type = "client_credentials";
        std::string api_key = "CZOJQKANIMA9LYSQ";
        std::string secret_key = "eb4392ac-b06c-4e60-b90e-16efea0b8827";
    } token;

    struct
    {
        std::string http_ip = "";
        std::string http_port;
    } uri;
} HttpConfig;

class httpClient
{
public:
    typedef enum _HttpError
    {
        HTTP_OK = 200,
        NO_REGISTER = 1313,
    } HttpError;

    httpClient(const HttpConfig &config);
    virtual ~httpClient();

    void init();

    bool faceIdentify(const std::string &file, std::string *response);
    bool faceIdentify(const uint8_t *data, const size_t size, std::string *response);
    bool token();

    static const int kTokenDurationTime_;

private:
    static std::string token_;
    std::string group_id_;
    static int64_t effect_time_;
    HttpError http_error_;
    static std::string check_code;
    static bool haveInit;
    HttpConfig config_;
};

inline size_t getUrlResponse(void *buffer, size_t size, size_t count, void *response)
{
    std::string *str = (std::string *) response;
    (*str).append((char *) buffer, size * count);

    return size * count;
}

} // namespace http
} // namespace jump

#endif // JUMP_HTTP_HTTPCLIENT_H
