#ifndef IMAGE_H
#define IMAGE_H

#include <opencv2/opencv.hpp>
#include <QImage>

namespace image
{
extern QImage mat2qim(cv::Mat &mat);
extern std::string img2base64(std::string filename, std::string ext = "jpeg");
extern QRect rect2qrect(const cv::Rect rect);
extern cv::Mat QImageToCvMat(const QImage &inImage, bool inCloneImageData = true);
extern cv::Mat QPixmapToCvMat(const QPixmap &inPixmap, bool inCloneImageData = true);
extern void rotate_arbitrarily_angle(cv::Mat &src, cv::Mat &dst, float angle);
} // namespace image

#endif // IMAGE_H
