#ifndef JUMP_CORE_PRODUCER_RECORD_IMPL_H
#define JUMP_CORE_PRODUCER_RECORD_IMPL_H

#include "consumer_base.h"
#include "producer_base.h"
#include <thread>
#include <vector>

namespace cv
{
class Mat;
}

class ProducerRecordImpl : public ProducerRecord
{
public:
    ProducerRecordImpl(int videoIndex = 0);
    ~ProducerRecordImpl();

    void startServer() override;
    void stopServer() override;

    void runInMainThread() { run(); }

private:
    void init();
    void run();

    bool running_;
    std::thread *thread_;
    int videoIndex_;
};

#endif // JUMP_CORE_PRODUCER_RECORD_IMPL_H
