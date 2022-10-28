#include "consumer_mjpeg.h"
#include "base/logging.h"
#include "mjpeg_writer.h"
// #include "muduo/base/Timestamp.h"
#include "base/time.h"
#include "opencv2/opencv.hpp"
#include "opencv_draw.h"

static int const g_max_continuous_count = 90;

ConsumerMjpeg::ConsumerMjpeg()
    : running_(false)
    , continuous_count_(0)
{
    color_mat_.release();
}

ConsumerMjpeg::~ConsumerMjpeg()
{
    LOG_DEBUG("exiting!!!");
    if (running_)
    {
        stop();
    }
    LOG_DEBUG("exited!!!");
}

void ConsumerMjpeg::init()
{
    pThread_ = new std::thread(&ConsumerMjpeg::handle, this);
}

void ConsumerMjpeg::start() { init(); }

void ConsumerMjpeg::stop()
{
    running_ = false;
    pThread_->join();

    delete pThread_;
    pThread_ = nullptr;
}

void ConsumerMjpeg::handle()
{
    running_ = true;

    // const auto window_name = "ConsumerMjpeg Image1";
    // const auto window_name2 = "ConsumerMjpeg Image2";
    // namedWindow(window_name, cv::WINDOW_AUTOSIZE);
    // namedWindow(window_name2, cv::WINDOW_AUTOSIZE);
    // std::cout << "here" << std::endl;
    cv::Mat color_mat;
    while (running_)
    {
        {
            std::unique_lock<std::mutex> record_unique(record_mutex_);
            color_mat = color_mat_.clone();
        }
        if (color_mat_.empty())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        {
            std::unique_lock<std::mutex> person_unique(person_mutex_);

            // LOG_DEBUG("MJPEG target:{}, id: {}, [x: {}, y: {}, w:{}, h: {}]",
            //           person_.has_target,
            //           person_.id,
            //           person_.rect.x,
            //           person_.rect.y,
            //           person_.rect.width,
            //           person_.rect.height);

            using namespace jump::time;
            static int count = 0;
            static int64_t start_time = getCurrentMilliTime();

            if (getCurrentMilliTime() - start_time > 1000)
            {
                int diff_time = getCurrentMilliTime() - start_time;
                int fps = count * 1000 / diff_time;
                LOG_DEBUG("MJPEG FPS: {}", fps);
                count = 0;
                start_time = getCurrentMilliTime();
            }
            else
            {
                ++count;
            }

            // 绘制人体框处理
            if (!person_.rect.empty())
            {
                // LOG_DEBUG("has person!");
                if (person_.has_target)
                {
                    if (continuous_count_ >= g_max_continuous_count)
                    {
                        person_.reset();
                    }
                    else
                    {
                        ++continuous_count_;
                    }
                    cv::rectangle(
                        color_mat, person_.rect, cv::Scalar(115, 210, 22), 8);
                }
                else
                {
                    cv::rectangle(
                        color_mat, person_.rect, cv::Scalar(0, 0, 255), 8);
                }
            }
            else
            {
                // LOG_DEBUG("no person!");
            }
        }

        jump::mjpeg::MJPEGWriter::appendNewFrame(color_mat);

        // 置空
        color_mat.release();
        if (0)
        {
            depth_mat_.release();
        }
    }
}

void ConsumerMjpeg::consumeRecord(cv::Mat color_mat, cv::Mat depth_mat)
{
    //  int64_t t1 = jump::time::getCurrentMilliTime();
    std::unique_lock<std::mutex> record_unique(record_mutex_);
    if (!color_mat_.empty())
    {
        color_mat_.release();
    }
    //    int64_t t2 = jump::time::getCurrentMilliTime();
    color_mat_ = color_mat.clone();
    // int64_t t3 = jump::time::getCurrentMilliTime();
    // LOG_DEBUG("mjpeg record: {}, {}",
    //           t2 - t1,
    //           jump::time::getCurrentMilliTime() - t2);
}

void ConsumerMjpeg::consumeJump(const jump::model::PersonIdentify person)
{
    std::unique_lock<std::mutex> unique(person_mutex_, std::try_to_lock);
    continuous_count_ = 0;
    if (unique.owns_lock()) // 如果拿到锁
    {
        person_ = person;
    }
}