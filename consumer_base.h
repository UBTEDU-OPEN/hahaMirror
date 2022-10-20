#ifndef JUMP_CORE_CONSUMER_BASE_H
#define JUMP_CORE_CONSUMER_BASE_H

#include <functional>
#include <opencv2/opencv.hpp>

class ConsumerBase
{
public:
    ConsumerBase() = default;
    ~ConsumerBase() = default;

    virtual void start() = 0;
    virtual void stop() = 0;
};

class ConsumerRecord : public ConsumerBase
{
public:
    ConsumerRecord() = default;
    ~ConsumerRecord() = default;

    virtual void consumeRecord(const cv::Mat color_mat, const cv::Mat depth_mat) = 0;

    // virtual void start() = 0;
    // virtual void stop() = 0;

    // protected:
    //     virtual void handle() = 0;
};

#endif // JUMP_CORE_CONSUMER_BASE_H
