#include "consumer_identify.h"
#include "common/logging.h"
#include "consumer_mjpeg.h"

const std::string ConsumerIdentify::window_name = "ConsumerIdentify Image1";
const std::string ConsumerIdentify::window_name2 = "ConsumerIdentify Image2";
std::vector<ConsumerIdentify::model_get_record_func> ConsumerIdentify::model_funcs;

ConsumerIdentify::ConsumerIdentify()
    : running_(false)
    , is_new_jump_(false)
{
}

ConsumerIdentify::~ConsumerIdentify()
{
    LOG_DEBUG("exiting!!!");
    if (running_)
    {
        stop();
    }
    LOG_DEBUG("exited!!!");
}

void ConsumerIdentify::stop()
{
    running_ = false;
    if (recordThread_ && recordThread_->joinable())
    {
        recordThread_->join();
    }
};

void ConsumerIdentify::init()
{
    recordThread_ = new std::thread(&ConsumerIdentify::handleRecord, this);
}

void ConsumerIdentify::sendRecordToConsumers()
{
    // std::unique_lock<std::mutex> producer_unique(producer_record_mutex_);

    // producer_color_mat_ = color_mat_.clone();
    // producer_depth_mat_ = depth_mat_.clone();

    auto consumers = getRecordConsumers();
    for (auto it = consumers.begin(); it != consumers.end(); ++it)
    {
        // std::cout << "[ConsumerIdentify] send record to consumers" <<
        // std::endl;
        (*it)->consumeRecord(color_mat_, depth_mat_);
    }

    // auto color_mat = color_mat_.clone();
    // auto depth_mat = depth_mat_.clone();
}

void ConsumerIdentify::sendRecordToJump()
{
    // 逻辑处理
    // cv::imshow(window_name, color_mat_);
    // cv::imshow(window_name2, depth_mat_);
    for (auto it = model_funcs.begin(); it != model_funcs.end(); ++it)
    {
        // std::cout << "[ConsumerIdentify] send record to model" << std::endl;
        (*it)(color_mat_, depth_mat_);
    }

    // 置空
}

void ConsumerIdentify::handle()
{ // handleRecord();
}

void ConsumerIdentify::handleRecord()
{
    while (running_)
    {
        if (color_mat_.empty())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
        std::unique_lock<std::mutex> consumer_unique(record_mutex_);

        sendRecordToJump();
        // LOG_DEBUG("once send");
        if (is_new_jump_)
        {
            sendRecordToConsumers();
        }

        color_mat_.release();
        is_new_jump_ = false;
    }
}

void ConsumerIdentify::consumeRecord(cv::Mat color_mat, cv::Mat depth_mat)
{
    std::unique_lock<std::mutex> unique(record_mutex_, std::try_to_lock);
    if (unique.owns_lock()) // 如果拿到锁
    {
        color_mat_ = color_mat.clone();
        depth_mat_ = depth_mat.clone();
    }
}

void ConsumerIdentify::consumeJump(const jump::model::PersonIdentify person)
{
    // LOG_DEBUG("fill person");
    jump_result_ = person;
    is_new_jump_ = true;
}
