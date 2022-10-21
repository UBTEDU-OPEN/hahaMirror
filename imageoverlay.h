#ifndef IMAGEOVERLAY_H
#define IMAGEOVERLAY_H

#include <opencv2/opencv.hpp>
#include <QObject>

class ImageOverlay : public QObject
{
    Q_OBJECT
public:
    ImageOverlay();

    void overlayImage(cv::Mat src_mat, const cv::Mat overlay_mat, const cv::Rect rect);
};

#endif // IMAGEOVERLAY_H
