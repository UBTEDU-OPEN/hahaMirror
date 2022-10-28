#include "hahacore.h"
#include "common/logging.h"
#include "common/time.h"
#include "facedetect.h"
#include "main_haha/include/faceHaha/face_haha_api.h"

Hahacore::Hahacore()
    : running_(false)
    , effect_(Face)
    , taskThread_(nullptr)
{
}

Hahacore::~Hahacore() {}

void Hahacore::start()
{
    running_ = true;
    init();
}

void Hahacore::stop()
{
    running_ = false;
    if (taskThread_ && taskThread_->joinable())
    {
        taskThread_->join();
    }
}

void Hahacore::init()
{
    taskThread_ = new std::thread(std::bind(&Hahacore::handleTaskCallback, this));
}

void Hahacore::handleTaskCallback()
{
    using namespace common::time;
    TimeConsumingAnalysis analysis;

    while (running_)
    {
        sourceMatMutex_.lock();
        if (sourceMat_.empty())
        {
            sourceMatMutex_.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        uint64_t begin_point_time = getCurrentMilliTime();

        {
            static int count = 0;
            static int64_t start_time = getCurrentMilliTime();

            ++count;
            if (getCurrentMilliTime() - start_time > 1000)
            {
                int diff_time = getCurrentMilliTime() - start_time;
                int fps = count * 1000 / diff_time;
                LOG_TRACE("hahaCore FPS: {}", fps);
                count = 0;
                start_time = getCurrentMilliTime();
            }
        }
        //   LOG_DEBUG("hahaMirror");
        analysis.addTimePoint();

        cv::Mat mat = sourceMat_.clone();
        sourceMatMutex_.unlock();

        analysis.addTimePoint("getSourceMatmutex and clone");

        effectMutex_.lock();
        int effect = effect_;
        effectMutex_.unlock();

        analysis.addTimePoint("getEffectmutex");

        auto results = faceDetectObj_->getCurrentPersonResults();

        analysis.addTimePoint("getFaceDetectResults");

        for (auto &oneFace : results)
        {
            // 0：眼睛； 1：嘴； 2：脸颊；
            if (effect_ != None)
            {
                HaHaFaceVision::UBT_AIFaceHaha(mat,
                                               mat,
                                               oneFace.algorithmFaceRect,
                                               oneFace.faceShape,
                                               effect);
            }
        }

        analysis.addTimePoint("haha handle finished!!");
        analysis.reset();
        // LOG_DEBUG(analysis.print());
        emit sig_sendHahaMat(mat.clone());

        mat.release();

        uint64_t end_point_time = getCurrentMilliTime();
        uint64_t diff = end_point_time - begin_point_time;
        if (diff < 30)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(30 - diff));
        }
    }
}

void Hahacore::consumeRecord(const cv::Mat color_mat, const cv::Mat original_mat)
{
    std::unique_lock<std::mutex> guard(sourceMatMutex_);

    if (!sourceMat_.empty())
    {
        sourceMat_.release();
    }

    //    using namespace common::time;
    //    TimeConsumingAnalysis analysis;
    //    analysis.addTimePoint();
    //    LOG_DEBUG(analysis.print());

    sourceMat_ = color_mat.clone();
}
