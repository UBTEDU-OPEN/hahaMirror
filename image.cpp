#include "image.h"
#include <opencv2/opencv.hpp>

namespace image
{
QImage mat2qim(cv::Mat &mat)
{
    cvtColor(mat, mat, cv::COLOR_BGR2RGB);
    QImage qim((const unsigned char *) mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
    return qim;
}
} // namespace image
