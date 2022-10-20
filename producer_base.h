#ifndef JUMP_CORE_PRODUCER_BASE_H
#define JUMP_CORE_PRODUCER_BASE_H

#include "consumer_base.h"
#include <functional>

class ProducerBase
{
public:
    ProducerBase() = default;
    ~ProducerBase() = default;
    ProducerBase(ProducerBase &) = delete;

    virtual void startServer() = 0;
    virtual void stopServer() = 0;
};

template<typename T, typename... U>
size_t getAddress(std::function<T(U...)> f)
{
    typedef T(fnType)(U...);
    fnType **fnPointer = f.template target<fnType *>();
    return (size_t) *fnPointer;
}

class ProducerRecord : public ProducerBase
{
public:
    typedef std::function<void(const cv::Mat, const cv::Mat)> consumerFunc;
    ProducerRecord() = default;
    ~ProducerRecord() = default;

    void registerRecordConsumer(ConsumerRecord *consumer)
    {
        for (auto it = consumers_.begin(); it != consumers_.end(); ++it)
        {
            // c++ 20 之前不能对 std::function 直接比较
            if (*it == consumer)
            {
                return;
            }
        }
        // std::cout << "registerRecordConsumer" << std::endl;
        consumers_.push_back(consumer);
    }

    void unRegisterRecordConsumer(ConsumerRecord *consumer)
    {
        auto func = consumerFunc(std::bind(&ConsumerRecord::consumeRecord,
                                           consumer,
                                           std::placeholders::_1,
                                           std::placeholders::_2));
        for (auto it = consumers_.begin(); it != consumers_.end(); ++it)
        {
            // c++ 20 之前不能对 std::function 直接比较
            if (*it == consumer)
            {
                consumers_.erase(it);
                return;
            }
        }
    }

    std::vector<ConsumerRecord *> &getRecordConsumers() { return consumers_; }

private:
    std::vector<ConsumerRecord *> consumers_;
};

#endif // JUMP_CORE_PRODUCER_BASE_H
