#ifndef BRIEFHTTPCLIENT_H
#define BRIEFHTTPCLIENT_H

#include <string.h>
#include <string>

class BriefHttpClient
{
public:
    BriefHttpClient(std::string url)
        : url_(url){};
    ~BriefHttpClient() = default;

    bool faceIdentify(const uint8_t *data, const size_t size, std::string &response);
    bool saveFaceRecord(const uint8_t *data, const size_t size, std::string &response);

private:
    std::string url_;
};

#endif // BRIEFHTTPCLIENT_H
