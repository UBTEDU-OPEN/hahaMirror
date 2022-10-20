#ifndef JUMP_CORE_CONSUMER_MJPEG_H
#define JUMP_CORE_CONSUMER_MJPEG_H

#include "consumer_base.h"
#include "model/model_demo.h"
#include "producer_base.h"
#include "producer_record_impl.h"
#include <mutex>
#include <thread>

class ConsumerMjpeg : public ConsumerRecord, public ConsumerJump
{
public:
    ConsumerMjpeg();
    ~ConsumerMjpeg();

    void start();
    void stop();

    void consumeRecord(const cv::Mat color_mat,
                       const cv::Mat depth_mat) override;
    void consumeJump(const jump::model::PersonIdentify person) override;

    void test() { handle(); }

protected:
    virtual void handle() override;

protected:
    void init();

    std::thread* pThread_;
    bool running_;
    // 视频流处理
    std::mutex record_mutex_;
    cv::Mat color_mat_;
    cv::Mat depth_mat_;
    // 识别结果处理
    std::mutex person_mutex_;
    jump::model::PersonIdentify person_;
    int continuous_count_;
};

#endif // JUMP_CORE_CONSUMER_MJPEG_H