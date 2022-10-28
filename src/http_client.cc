#include "http_client.h"

#include <iostream>

#include "common/json.hpp"
#include "common/logging.h"
#include "common/time.h"

using namespace nlohmann;
using namespace jump::http;

static std::string gMinFaceSize = "10";
const int httpClient::kTokenDurationTime_ = 300;

namespace jump
{
namespace http
{
int64_t httpClient::effect_time_ = 0;
std::string httpClient::token_ = "";
std::string httpClient::check_code
    = "grantType=grantType-default&apiKey=apiKey-default&secretKey=secretKey-"
      "default";
bool httpClient::haveInit = false;
} // namespace http
} // namespace jump

httpClient::httpClient(const HttpConfig &config)
    : config_(config)
{
    if (!haveInit)
    {
        init();
    }
}

httpClient::~httpClient() {}

void httpClient::init()
{
    auto token_config = config_.token;
    size_t pos = 0;

    pos = check_code.find("grantType-default");
    check_code = check_code.replace(pos, strlen("grantType-default"), token_config.grant_type);
    pos = check_code.find("apiKey-default");
    check_code = check_code.replace(pos, strlen("apiKey-default"), token_config.api_key);
    pos = check_code.find("secretKey-default");
    check_code = check_code.replace(pos, strlen("secretKey-default"), token_config.secret_key);

    haveInit = true;
}

bool httpClient::token()
{
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
        curl_easy_setopt(curl, CURLOPT_URL, std::string(config_.url + config_.record.token).c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");

        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, check_code.c_str());

        std::string response;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &http::getUrlResponse);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        if (res == CURLE_OK)
        {
            try
            {
                LOG_DEBUG("response:{}", response);
                auto j = json::parse(response);
                int code = j["resultCode"].get<int>();

                if (code == HTTP_OK)
                {
                    auto data = j["data"];
                    token_ = data["accessToken"].get<std::string>();
                    uint64_t current_time = common::time::getCurrentTime();
                    effect_time_ = current_time + kTokenDurationTime_;
                    LOG_DEBUG("token:{} effect_time:{}", token_, effect_time_);
                    return true;
                }
            }
            catch (const json::parse_error &error)
            {
                LOG_WARN("[token] parse_error: {}", error.what());
                return false;
            }
            catch (const json::type_error &error)
            {
                LOG_WARN("[token] parse_error: {}", error.what());
                return false;
            }

            return false;
        }
    }

    effect_time_ = 0;
    return false;
}

// 还需要对 http 错误请求的场景进行判断

bool httpClient::faceIdentify(const std::string &file, std::string *response)
{
    uint64_t current_time = common::time::getCurrentTime();
    if (current_time > (uint64_t) effect_time_)
    {
        bool status = token();
        if (!status)
        {
            return false;
        }
    }

    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
        curl_easy_setopt(curl,
                         CURLOPT_URL,
                         std::string(config_.url + config_.record.identify).c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");

        struct curl_slist *headers = NULL;
        std::string accessToken = "accessToken:" + token_;
        headers = curl_slist_append(headers, accessToken.c_str());
        headers = curl_slist_append(headers, "Content-Type: multipart/form-data");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_mime *mime;
        curl_mimepart *part;
        mime = curl_mime_init(curl);
        part = curl_mime_addpart(mime);
        std::string group_id = config_.group.fixed ? config_.group.id
                                                   : common::time::timeFormatYMD();
        curl_mime_name(part, "groupId");
        curl_mime_data(part, group_id.c_str(), CURL_ZERO_TERMINATED);
        part = curl_mime_addpart(mime);
        curl_mime_name(part, "image");
        curl_mime_filedata(part, file.c_str());
        curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);

        std::string buf;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &http::getUrlResponse);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buf);
        res = curl_easy_perform(curl);
        curl_mime_free(mime);

        if (res == CURLE_OK)
        {
            LOG_DEBUG(buf);
            *response = buf;
            return true;
        }
        return false;
    }
    return false;
}

bool httpClient::faceIdentify(const uint8_t *data, const size_t size, std::string *response)
{
    uint64_t current_time = common::time::getCurrentTime();

    LOG_DEBUG("current-time: {}, token-effect-endtime:{}", current_time, effect_time_);

    if (current_time > (uint64_t) effect_time_)
    {
        bool status = token();
        if (!status)
        {
            return false;
        }
    }

    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
        curl_easy_setopt(curl,
                         CURLOPT_URL,
                         std::string(config_.url + config_.record.identify).c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");

        struct curl_slist *headers = NULL;
        std::string accessToken = "accessToken:" + token_;
        headers = curl_slist_append(headers, accessToken.c_str());
        headers = curl_slist_append(headers, "Content-Type: multipart/form-data");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        struct curl_httppost *post = NULL;
        struct curl_httppost *last = NULL;
        std::string group_id = config_.group.fixed ? config_.group.id
                                                   : common::time::timeFormatYMD();

        // LOG_DEBUG("fixed: {}, group_id: {}, timeformat: {}",
        //           config_.group.fixed,
        //           config_.group.id,
        //           jump::time::timeFormatYMD());
        curl_formadd(&post,
                     &last,
                     CURLFORM_COPYNAME,
                     "groupId",
                     CURLFORM_COPYCONTENTS,
                     group_id.c_str(),
                     CURLFORM_END); //  impl::timeFormatYMD().c_str()
        curl_formadd(&post,
                     &last,
                     CURLFORM_COPYNAME,
                     "faceMinSize",
                     CURLFORM_COPYCONTENTS,
                     gMinFaceSize.c_str(),
                     CURLFORM_END);
        curl_formadd(&post,
                     &last,
                     CURLFORM_COPYNAME,
                     "image",
                     CURLFORM_BUFFER,
                     "data",
                     CURLFORM_BUFFERPTR,
                     data,
                     CURLFORM_BUFFERLENGTH,
                     size,
                     CURLFORM_END);
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &http::getUrlResponse);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res == CURLE_OK)
        {
            LOG_INFO("success: {}", *response);
        }
        else
        {
            LOG_ERROR("failed: {}", *response);
        }
    }
    return false;
}
