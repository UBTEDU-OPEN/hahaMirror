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

    int id = 1;

    using namespace common::time;

    VideoCapture cap(videoIndex_);
    if (!cap.isOpened())
    {
        cout << "Cannot open camera\n";
        abort();
    }

    Mat frame;
    Mat gray;
    //namedWindow("live", WINDOW_AUTOSIZE); // 命名一個視窗，可不寫

    while (running_)
    {
        // 擷取影像
        bool ret = cap.read(frame); // or cap >> frame;
        if (!ret)
        {
            cout << "Can't receive frame (stream end?). Exiting ...\n";
            break;
        }

        // 彩色轉灰階
        cvtColor(frame, gray, COLOR_BGR2GRAY);
        // 按下 q 鍵離開迴圈
        if (waitKey(1) == 'q')
        {
            break;
        }

        auto consumers = getRecordConsumers();
        for (auto it = consumers.begin(); it != consumers.end(); ++it)
        {
            // LOG_DEBUG("send");
            (*it)->consumeRecord(frame, gray);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

ProducerRecordImpl::ProducerRecordImpl()
    : running_(false)
    , thread_(nullptr)
    , videoIndex_(0)
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