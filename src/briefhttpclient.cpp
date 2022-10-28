#include "briefhttpclient.h"

#include "http_client.h"

#include <iostream>

#include "common/json.hpp"
#include "common/logging.h"
#include "common/time.h"

static size_t WriteCallback(char *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *) userp)->append((char *) contents, size * nmemb);
    return size * nmemb;
}

bool BriefHttpClient::faceIdentify(const uint8_t *data, const size_t size, std::string &response)
{
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
        curl_easy_setopt(curl, CURLOPT_URL, url_.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");

        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: multipart/form-data");
        headers = curl_slist_append(headers, "accept: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        struct curl_httppost *post = NULL;
        struct curl_httppost *last = NULL;

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
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res == CURLE_OK)
        {
            LOG_INFO("success: {}", response);
            return true;
        }
        else
        {
            LOG_ERROR("failed: {}", response);
        }
    }
    return false;
}
