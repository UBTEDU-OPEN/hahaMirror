#include "resolution.h"
#include "common/logging.h"

const MarginScale Resolution::kMirrorMarginScale_{0.0991, 0.1443, 0.8018, 0.7114};
const int Resolution::kRobotRectsCount_ = 3;
const std::vector<MarginScale> Resolution::kRobotMarginScale_{{0.0219, 0.2943, 0.2981, 0.1917},
                                                              {0.7526, 0.2214, 0.2981, 0.1917},
                                                              {0.0991, 0.1443, 0.9009, 0.8557},
                                                              {0.0991, 0.1443, 0.9009, 0.8557}};
const MarginScale Resolution::kTipsMarginScale_{0.1092, 0.0458, 0.7816, 0.0792};

void Resolution::update(cv::Size resolution)
{
    int width = resolution.width;
    int height = resolution.height;
    // LOG_DEBUG("resolution: {},{}", width, height);

    std::vector<cv::Rect> robotRects;
    for (int i = 0; i < kRobotRectsCount_; ++i)
    {
        cv::Rect robot_rect;
        robotRects.emplace_back(robot_rect);
    }

    std::lock_guard<std::mutex> guard(mutex_);
    resolution_ = resolution;
    mirrorMarginRect_ = cv::Rect(width * kMirrorMarginScale_.left,
                                 height * kMirrorMarginScale_.up,
                                 width * kMirrorMarginScale_.width,
                                 height * kMirrorMarginScale_.height);
    robotRects_.swap(robotRects);

    //    LOG_DEBUG("tips scale: {},{},{},{}",
    //              kTipsMarginScale_.left,
    //              kTipsMarginScale_.up,
    //              kTipsMarginScale_.width,
    //              kTipsMarginScale_.height);
    tipsMarginRect_ = cv::Rect(width * kTipsMarginScale_.left,
                               height * kTipsMarginScale_.up,
                               width * kTipsMarginScale_.width,
                               height * kTipsMarginScale_.height);

    //    LOG_DEBUG("tips rect: {},{},{},{}",
    //              tipsMarginRect_.x,
    //              tipsMarginRect_.y,
    //              tipsMarginRect_.width,
    //              tipsMarginRect_.height);
}
