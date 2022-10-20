#ifndef JUMP_CORE_CONSUMER_IDENTIFY_H
#define JUMP_CORE_CONSUMER_IDENTIFY_H

#include "consumer_base.h"
#include "model/model_demo.h"
#include "producer_base.h"
#include <thread>

class ConsumerIdentify
    : public ConsumerRecord,
      public ConsumerJump,
      public ProducerRecord, // 只有经过算法处理过的视频流才需要传输
      public ProducerJump
{
public:
    typedef std::function<void(const cv::Mat color_mat,
                               const cv::Mat depth_mat)>
        model_get_record_func;
    ConsumerIdentify();
    ~ConsumerIdentify();
    // consumer
    void start() override
    {
        running_ = true;
        init();
    }
    void stop() override;

    void consumeRecord(const cv::Mat color_mat,
                       const cv::Mat depth_mat) override;
    void consumeJump(const jump::model::PersonIdentify person);

    // producer
    void startServer() override{};
    void stopServer() override{};

    static void registerFunc(model_get_record_func func)
    {
        model_funcs.push_back(func);
    }

protected:
    virtual void handle() override;
    void handleRecord();

private:
    void init();
    void sendRecordToConsumers();
    void sendRecordToJump();

    bool running_;
    bool is_new_jump_;

    std::thread* recordThread_;
    std::mutex record_mutex_;
    cv::Mat color_mat_;
    cv::Mat depth_mat_;

    jump::model::PersonIdentify jump_result_;

    static const std::string window_name;
    static const std::string window_name2;

    static std::vector<model_get_record_func> model_funcs;
};

#endif // JUMP_CORE_CONSUMER_IDENTIFY_H