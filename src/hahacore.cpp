#include "hahacore.h"
#include "common/logging.h"
#include "common/time.h"
#include "facedetect.h"
#include "main_haha/include/faceHaha/face_haha_api.h"

Hahacore::Hahacore()
    : running_(false)
    , effect_(Face)
    , taskThread_(nullptr)
    , faceDetectObj_(0)
    , interval_time_(1000 / 24)
    , beauty_(0)
{
    connect(&timer_, &QTimer::timeout, this, &Hahacore::slot_timeout);
}

Hahacore::~Hahacore() {}

void Hahacore::start()
{
    running_ = true;

    timer_.setInterval(interval_time_);
    timer_.start(); // 优化项，就是需要一个更精准的定时器

    // init();
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

    if (timer_.isActive())
    {
        timer_.stop();
    }
}

void Hahacore::init()
{
    taskThread_ =
        new std::thread(std::bind(&Hahacore::handleTaskCallback, this));
}

void Hahacore::slot_timeout()
{
    using namespace common::time;

    sourceMatMutex_.lock();
    if (faceDetectResult_.empty() && sourceMat_.empty())
    {
        // std::cout << "continue" << std::endl;
        sourceMatMutex_.unlock();
        return;
    }
    sourceMatMutex_.unlock();

    static common::time::CaculateFps fps;
    std::string rate = fps.add();
    if (rate != "")
    {
        LOG_DEBUG("Hahacore FPS: {}!!", rate);
    }

    TimeConsumingAnalysis analysis;

    sourceMatMutex_.lock();
    cv::Mat mat = sourceMat_.clone();
    std::vector<FaceDetectResult> results;
    results.swap(faceDetectResult_);
    sourceMat_.release();
    sourceMatMutex_.unlock();

    analysis.addTimePoint("getSourceMatmutex and clone");

    effectMutex_.lock();
    int effect = effect_;
    effectMutex_.unlock();

    for (auto& oneFace : results)
    {
        // 0：眼睛； 1：嘴； 2：脸颊；
        if (effect_ != None)
        {
            HaHaFaceVision::UBT_AIFaceHaha(mat,
                                           mat,
                                           oneFace.algorithmFaceRect,
                                           oneFace.faceShape,
                                           effect,
                                           beauty_);
            // cv::rectangle(mat, oneFace.algorithmFaceRect, cv::Scalar(0, 0,
            // 255), 2);
        }
    }

    analysis.addTimePoint("haha handle finished!!");
    LOG_TRACE(analysis.print());
    //  cv::imshow("haha", mat);
    emit sig_sendHahaMat(mat.clone());

    mat.release();
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
        sourceMatMutex_.unlock();
    }

    while (running_)
    {
        static common::time::CaculateFps fps;
        std::string rate = fps.add();
        if (rate != "")
        {
            LOG_DEBUG("Hahacore FPS: {}!!", rate);
        }

        uint64_t begin_point_time = getCurrentMilliTime();

        TimeConsumingAnalysis analysis;
        sourceMatMutex_.lock();
        cv::Mat mat = sourceMat_.clone();
        sourceMatMutex_.unlock();

        analysis.addTimePoint("getSourceMatmutex and clone");

        effectMutex_.lock();
        int effect = effect_;
        effectMutex_.unlock();

        analysis.addTimePoint("getEffectmutex");

        auto results = faceDetectObj_->getCurrentPersonResults();

        analysis.addTimePoint("getFaceDetectResults");

        for (auto& oneFace : results)
        {
            // 0：眼睛； 1：嘴； 2：脸颊；
            if (effect_ != None)
            {
                HaHaFaceVision::UBT_AIFaceHaha(mat,
                                               mat,
                                               oneFace.algorithmFaceRect,
                                               oneFace.faceShape,
                                               effect,
                                               beauty_);
            }
        }

        analysis.addTimePoint("haha handle finished!!");
        LOG_TRACE(analysis.print());
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

void Hahacore::setFaceInfo(cv::Mat& mat, std::vector<FaceDetectResult> result)
{
    common::time::TimeConsumingAnalysis anal;
    sourceMatMutex_.lock();
    anal.addTimePoint("getMutex");
    sourceMat_ = mat.clone();
    anal.addTimePoint("cloneMat");
    if (result.size() > 0)
    {
        faceDetectResult_.swap(result);
    }
    anal.addTimePoint("swapResult");
    sourceMatMutex_.unlock();

    LOG_TRACE(anal.print());
}

void Hahacore::consumeRecord(const cv::Mat color_mat,
                             const cv::Mat original_mat)
{
    std::unique_lock<std::mutex> guard(sourceMatMutex_);

    if (!sourceMat_.empty())
    {
        sourceMat_.release();
    }

    sourceMat_ = color_mat.clone();
}
