#include "producer_record_impl.h"
#include "common/logging.h"
#include "common/time.h"
#include <librealsense2/rs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv4/opencv2/dnn.hpp>
#include <stdio.h>
// #include <opencv4/opencv2/opencv.hpp>
using namespace cv;
using namespace cv::dnn;
using namespace rs2;
using namespace std;

void ProducerRecordImpl::run()
{
    running_ = true;

    VideoCapture cap(videoIndex_, cv::CAP_ANY);
    // cap.set(cv::CAP_PROP_FRAME_WIDTH, 1920);
    //  cap.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);

    if (!cap.isOpened())
    {
        cout << "Cannot open camera\n";
        abort();
    }

    Mat frame;

    while (running_)
    {
        // 擷取影像
        bool ret = cap.read(frame); // or cap >> frame;
        cv::flip(frame, frame, 1);  // 0: x轴翻转  1: y轴翻转  2: 同时翻转
        // imshow("image", frame);
        if (!ret)
        {
            LOG_ERROR("Can't receive frame (stream end?). Exiting ...");
            break;
        }
        LOG_TRACE("[frame] w: {}, h:{}", frame.cols, frame.rows);
        auto consumers = getRecordConsumers();
        for (auto it = consumers.begin(); it != consumers.end(); ++it)
        {
            (*it)->consumeRecord(frame, frame);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    running_ = false;
}

ProducerRecordImpl::ProducerRecordImpl(int videoIndex)
    : running_(false)
    , thread_(nullptr)
    , videoIndex_(videoIndex)
{
}

void ProducerRecordImpl::init()
{
    thread_ = new std::thread(&ProducerRecordImpl::run, this);
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
    thread_->join();
    delete thread_;
    thread_ = NULL;
}
