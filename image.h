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
}

#endif // IMAGE_H
