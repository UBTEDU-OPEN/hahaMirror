#include "playcamera.h"
#include "common/logging.h"
#include "image.h"

PlayCamera::PlayCamera()
    : running_(false)
    , taskThread_(nullptr)
{
}

PlayCamera::~PlayCamera()
{
    stop();
}

void PlayCamera::init()
{
    taskThread_ = new std::thread(std::bind(&PlayCamera::handleTaskCallback, this));
}

void PlayCamera::start()
{
    running_ = true;
    init();
}

void PlayCamera::stop()
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

void PlayCamera::handleTaskCallback()
{
    while (running_)
    {
        {
            matMutex_.lock();
            if (!curMat_.empty())
            {
                // QImage hahaImg = image::mat2qim(curMat_);
                // emit sig_getHahaImage(hahaImg);
                emit sig_getHahaImage1(curMat_.clone());
                curMat_.release();
                matMutex_.unlock();
            }
            else
            {
                matMutex_.unlock();
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }

        //
    }
}

void PlayCamera::consumeRecord(const cv::Mat color_mat, const cv::Mat original_mat)
{
    std::unique_lock<std::mutex> guard(matMutex_);
    if (!curMat_.empty())
    {
        curMat_.release();
    }
    curMat_ = color_mat.clone();
}
