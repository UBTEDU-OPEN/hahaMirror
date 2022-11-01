#ifndef RESOLUTION_H
#define RESOLUTION_H

#include <opencv2/opencv.hpp>

struct MarginScale
{
    double left;
    double up;
    double width;
    double height;

    MarginScale(double left, double up, double width, double height)
    {
        this->left = left;
        this->up = up;
        this->width = width;
        this->height = height;
    }
};

class Resolution
{
public:
    Resolution(cv::Size resolution) { update(resolution); }
    ~Resolution();

    void update(cv::Size resolution);

    static const MarginScale kMirrorMarginScale_;
    static const std::vector<MarginScale> kRobotMarginScale_;
    static const int kRobotRectsCount_;
    static const MarginScale kTipsMarginScale_;

private:
    std::mutex mutex_;

public:
    cv::Size resolution_;
    cv::Rect mirrorMarginRect_;
    std::vector<cv::Rect> robotRects_;
    cv::Rect tipsMarginRect_;
};

#endif // RESOLUTION_H
