#include "image.h"
#include <opencv2/opencv.hpp>
#include <QBuffer>
#include <QImage>
#include <QPixmap>

using namespace cv;

namespace image
{
QImage mat2qim(cv::Mat &mat)
{
    if (mat.type() == CV_8UC1)
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        image.setColorCount(
            256); //5.9版本之后取消了setNumColors函数，可以使用image.setColoeCount(256);
        for (int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar *pSrc = mat.data;
        for (int row = 0; row < mat.rows; row++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if (mat.type() == CV_8UC3)
    {
        // Copy input Mat
        // std::cout << "CV_8UC3" << std::endl;
        const uchar *pSrc = (const uchar *) mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if (mat.type() == CV_8UC4)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar *) mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    }
    else
    {
        return QImage();
    }
}

std::string img2base64(std::string filename, std::string ext)
{
    QImage image(QString::fromStdString(filename));
    QByteArray ba;
    QBuffer buf(&ba);
    image.save(&buf, ext.c_str());
    buf.close();

    return ba.toBase64().toStdString();
}

QRect rect2qrect(const cv::Rect rect)
{
    if (rect.empty())
    {
        return QRect();
    }

    return QRect(rect.x, rect.y, rect.width, rect.height);
}

cv::Mat QImageToCvMat(const QImage &inImage, bool inCloneImageData)
{
    // std::cout << inImage.format() << std::endl;
    switch (inImage.format())
    {
    // 8-bit, 4 channel
    case QImage::Format_ARGB32:
    case QImage::Format_ARGB32_Premultiplied:
    {
        cv::Mat mat(inImage.height(),
                    inImage.width(),
                    CV_8UC4,
                    const_cast<uchar *>(inImage.bits()),
                    static_cast<size_t>(inImage.bytesPerLine()));

        return (inCloneImageData ? mat.clone() : mat);
    }

    // 8-bit, 3 channel
    case QImage::Format_RGB32:
    case QImage::Format_RGB888:
    {
        if (!inCloneImageData)
        {
            std::cout << "CVS::QImageToCvMat() - Conversion requires cloning because we use a "
                         "temporary QImage"
                      << std::endl;
            ;
        }

        QImage swapped = inImage;

        if (inImage.format() == QImage::Format_RGB32)
        {
            swapped = swapped.convertToFormat(QImage::Format_RGB888);
        }

        swapped = swapped.rgbSwapped();

        return cv::Mat(swapped.height(),
                       swapped.width(),
                       CV_8UC3,
                       const_cast<uchar *>(swapped.bits()),
                       static_cast<size_t>(swapped.bytesPerLine()))
            .clone();
    }

    // 8-bit, 1 channel
    case QImage::Format_Indexed8:
    {
        cv::Mat mat(inImage.height(),
                    inImage.width(),
                    CV_8UC1,
                    const_cast<uchar *>(inImage.bits()),
                    static_cast<size_t>(inImage.bytesPerLine()));

        return (inCloneImageData ? mat.clone() : mat);
    }

    default:
        std::cout << "CVS::QImageToCvMat() - QImage format not handled in switch:"
                  << inImage.format() << std::endl;
        break;
    }

    return cv::Mat();
}

cv::Mat QPixmapToCvMat(const QPixmap &inPixmap, bool inCloneImageData)
{
    return QImageToCvMat(inPixmap.toImage(), inCloneImageData);
}

// model 1,2,3 分别对应着 90, 180, 270
void rotate_arbitrarily_angle(Mat &src, Mat &dst, float angle)
{
    float radian = (float) (angle / 180.0 * CV_PI);

    //填充图像
    int maxBorder = (int) (max(src.cols, src.rows) * 1.414); //即为sqrt(2)*max
    int dx = (maxBorder - src.cols) / 2;
    int dy = (maxBorder - src.rows) / 2;
    copyMakeBorder(src, dst, dy, dy, dx, dx, BORDER_CONSTANT);

    //旋转
    Point2f center((float) (dst.cols / 2), (float) (dst.rows / 2));
    Mat affine_matrix = getRotationMatrix2D(center, angle, 1.0); //求得旋转矩阵
    warpAffine(dst, dst, affine_matrix, dst.size());

    //计算图像旋转之后包含图像的最大的矩形
    float sinVal = abs(sin(radian));
    float cosVal = abs(cos(radian));
    Size targetSize((int) (src.cols * cosVal + src.rows * sinVal),
                    (int) (src.cols * sinVal + src.rows * cosVal));

    //剪掉多余边框
    int x = (dst.cols - targetSize.width) / 2;
    int y = (dst.rows - targetSize.height) / 2;
    Rect rect(x, y, targetSize.width, targetSize.height);
    dst = Mat(dst, rect);
}

} // namespace image
