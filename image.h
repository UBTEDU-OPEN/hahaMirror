#ifndef IMAGE_H
#define IMAGE_H

#include <QImage>

namespace cv
{
class Mat;
}

namespace image
{
extern QImage mat2qim(cv::Mat &mat);
extern std::string img2base64(std::string filename, std::string ext = "jpeg");
} // namespace image

#endif // IMAGE_H
