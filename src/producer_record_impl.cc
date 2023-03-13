#include "producer_record_impl.h"
#include "common/fileopt.h"
#include "common/logging.h"
#include "common/time.h"
#include "hahaui.h"
#include "image.h"
#include <librealsense2/rs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv4/opencv2/dnn.hpp>
#include <stdio.h>

using namespace cv;
using namespace std;

void ProducerRecordImpl::setResolution(cv::Size resolution)
{
    resolution_ = resolution;
}

void ProducerRecordImpl::sendEmptyMatToAllConsumers()
{
    Mat image(1080, 1920, CV_8UC3, 100);
    auto consumers = getRecordConsumers();
    for (auto it = consumers.begin(); it != consumers.end(); ++it)
    {
        (*it)->consumeRecord(image, image);
    }
}

void ProducerRecordImpl::run()
{
    using namespace common::time;

    running_ = true;

    // 发送一个空的Mat, 当摄像头打不开的时候也有待机界面
    sendEmptyMatToAllConsumers();

CaptureCamera:
    while (!common::fileopt::isExsit("/dev/video0"))
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        continue;
    }

    VideoCapture cap(videoIndex_, cv::CAP_ANY);
    cap.set(cv::CAP_PROP_FRAME_WIDTH, config_->camera()->width);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, config_->camera()->height);
    if (!cap.isOpened())
    {
        LOG_ERROR("Cannot open camera!!");
        cap.release();
        goto CaptureCamera;
    }

    Mat frame;
    Mat clone;
    while (running_)
    {
        TimeConsumingAnalysis anaylis;

        static common::time::CaculateFps fps;
        std::string rate = fps.add();
        if (rate != "")
        {
            LOG_DEBUG("Camera Fps: {}!!", rate);
        }

        // 擷取影像

        bool ret = cap.read(frame); // or cap >> frame;
        if (!ret)
        {
            LOG_ERROR("Can't receive frame (stream end?). Reconnecting ...");
            cap.release();
            goto CaptureCamera;
        }
        anaylis.addTimePoint("readframe");
        // LOG_DEBUG("before: {},{}", frame.cols, frame.rows);
        cv::flip(frame, frame, 1); // 0: x轴翻转  1: y轴翻转  2: 同时翻转
        anaylis.addTimePoint("flip");

        if (direction_ == 1)
        {
            image::rotate_arbitrarily_angle(frame, clone, 90);
            //    LOG_TRACE("rotate: w: {}, h: {}", frame.cols, frame.rows);
            anaylis.addTimePoint("rotate");
        }
        else if (direction_ == 0)
        {
            static int height = frame.rows;
            static int old_width = frame.cols;
            static int width = (height * height) / (old_width * 1.0);
            static int x = (old_width - width) / 2;
            static cv::Rect rect(x, 0, width, height);
            static cv::Size size(height, old_width);
            clone = frame(rect).clone();
            //            LOG_TRACE("size: src: {},{}, dest: {}, {}",
            //                      clone.cols,
            //                      clone.rows,
            //                      size.width,
            //                      size.height);

            anaylis.addTimePoint("cut out");
            cv::resize(clone, clone, size);
            anaylis.addTimePoint("resize");
        }

        auto consumers = getRecordConsumers();
        for (auto it = consumers.begin(); it != consumers.end(); ++it)
        {
            (*it)->consumeRecord(clone, clone);
        }

        anaylis.addTimePoint("send to consumers");
        LOG_TRACE(anaylis.print());
    }

    running_ = false;
}

ProducerRecordImpl::ProducerRecordImpl(int videoIndex)
    : running_(false)
    , thread_(nullptr)
    , videoIndex_(videoIndex)
    , hahaUi_(nullptr)
    , direction_(1)
{
}

void ProducerRecordImpl::init()
{
    thread_ = new std::thread(&ProducerRecordImpl::run, this);
    // detectCameraThread_ = new std::thread(&ProducerRecordImpl::detectCameraCallback, this);
}

void ProducerRecordImpl::startServer()
{
    init();
}

ProducerRecordImpl::~ProducerRecordImpl()
{
    //  std::cout << "[Record begin] exit" << std::endl;
    LOG_DEBUG("exiting!!!");
    if (running_)
    {
        stopServer();
    }
    // std::cout << "[Record end] exit" << std::endl;

    LOG_DEBUG("exited!!!");
}

void ProducerRecordImpl::stopServer()
{
    running_ = false;
    if (thread_)
    {
        if (thread_->joinable())
        {
            thread_->join();
        }

        delete thread_;
    }
    thread_ = nullptr;
}
