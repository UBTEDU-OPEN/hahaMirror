#include "playcamera.h"
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
    init();
}

void PlayCamera::stop()
{
    running_ = false;
    if (taskThread_ && taskThread_->joinable())
    {
        taskThread_->join();
    }
}

void PlayCamera::handleTaskCallback()
{
    while (running_)
    {
        {
            std::unique_lock<std::mutex> guard(matMutex_);
            if (!curMat_.empty())
            {
                hahaImg_ = image::mat2qim(curMat_);
            }
        }

        emit sig_getHahaImage(hahaImg_);

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
