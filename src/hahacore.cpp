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
    if (taskThread_)
    {
        if (taskThread_->joinable())
        {
            taskThread_->join();
        }

        delete taskThread_;
    }
}

void Hahacore::init()
{
    taskThread_ = new std::thread(std::bind(&Hahacore::handleTaskCallback, this));
}

void Hahacore::handleTaskCallback()
{
    using namespace common::time;

    while (running_)
    {
        sourceMatMutex_.lock();
        if (sourceMat_.empty())
        {
            sourceMatMutex_.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        std::string rate = common::time::caculateFPS();
        if (rate != "")
        {
            LOG_TRACE("HahaCore Fps: {}!!", rate);
        }

        uint64_t begin_point_time = getCurrentMilliTime();

        TimeConsumingAnalysis analysis;
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
            std::this_thread::sleep_for(
                std::chrono::milliseconds(config_->haha()->interval_time_ms - diff));
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
