#ifndef JUMP_CORE_PRODUCER_RECORD_IMPL_H
#define JUMP_CORE_PRODUCER_RECORD_IMPL_H

#include "config.h"
#include "consumer_base.h"
#include "producer_base.h"

#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

class HahaUi;

class ProducerRecordImpl : public ProducerRecord
{
public:
    ProducerRecordImpl(int videoIndex = 0);
    ~ProducerRecordImpl();

    void startServer() override;
    void stopServer() override;

    void runInMainThread() { run(); }
    void setHahaUi(HahaUi *hahaui) { hahaUi_ = hahaui; }
    void setResolution(cv::Size resolution);
    void setConfig(config::Config *config)
    {
        config_ = config;
        direction_ = config_->camera()->direction;
    }

private:
    void init();
    void run();
    void sendEmptyMatToAllConsumers();
    bool running_;
    std::thread *thread_;
    int videoIndex_;
    HahaUi *hahaUi_;
    cv::Size resolution_;
    config::Config *config_;
    int direction_;
};

#endif // JUMP_CORE_PRODUCER_RECORD_IMPL_H
