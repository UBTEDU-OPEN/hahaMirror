#include "image.h"
#include <opencv2/opencv.hpp>
#include <QBuffer>

namespace image
{
QImage mat2qim(cv::Mat &mat)
{
    cvtColor(mat, mat, cv::COLOR_BGR2RGB);
    QImage qim((const unsigned char *) mat.data,
               mat.cols,
               mat.rows,
               QImage::Format_RGB888); //  mat.step,
    return qim;
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

} // namespace image
