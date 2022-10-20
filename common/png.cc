#include "png.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

//#include <opencv4/opencv2/core.hpp>
#include <opencv2/opencv.hpp>

#include "logging.h"

namespace jump
{
namespace png
{
void cvMatToByte(const cv::Mat& mat, uint8_t* data, size_t* size, size_t maxsize)
{
    using namespace cv;

    *size = 0;

    std::vector<uchar> data_encode;
    try
    {
        std::vector<int> param = std::vector<int>(2);
        param[0] = cv::IMWRITE_PNG_COMPRESSION;
        param[1] = 3;
        imencode(".png", mat, data_encode, param);
    }
    catch (Exception& e)
    {
        const char* s_ERROR = e.what();
        std::string a(s_ERROR);
        int c = 0;
        c++;
    }

    //    LOG_DEBUG("data_encode size: {}", data_encode.size());

    if (data_encode.size() <= maxsize)
    {
        *size = data_encode.size();
        uint8_t* tem = &*data_encode.begin();
        memcpy(data, tem, *size);
    }
}

void byteToPNG(const uint8_t* data, const size_t size, const std::string& file)
{
    int fd = open(file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0766);
    if (fd != -1)
    {
        int len = write(fd, data, size);
        close(fd);
        if (len != size)
        {
            LOG_WARN("write file failed");
        }
    }
    else
    {
        LOG_ERROR("open file failed!");
    }
}

} // namespace png
} // namespace jump
