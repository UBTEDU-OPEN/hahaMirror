#include "producer_record_impl.h"
#include "common/logging.h"
#include "common/time.h"
#include "hahaui.h"
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

// model 1,2,3 分别对应着 90, 180, 270
static void rotate_arbitrarily_angle(Mat &src, Mat &dst, float angle)
{
    float radian = (float) (angle / 180.0 * CV_PI);

    //填充图像
    int maxBorder = (int) (max(src.cols, src.rows) * 1.414); //即为sqrt(2)*max
    int dx = (maxBorder - src.cols) / 2;
    int dy = (maxBorder - src.rows) / 2;
    copyMakeBorder(src, dst, dy, dy, dx, dx, BORDER_CONSTANT);

    //旋转
    Point2f center((float) (dst.cols / 2), (float) (dst.rows / 2));
    Mat affine_matrix = getRotationMatrix2D(center, angle, 1.0); //求得旋转矩阵
    warpAffine(dst, dst, affine_matrix, dst.size());

    //计算图像旋转之后包含图像的最大的矩形
    float sinVal = abs(sin(radian));
    float cosVal = abs(cos(radian));
    Size targetSize((int) (src.cols * cosVal + src.rows * sinVal),
                    (int) (src.cols * sinVal + src.rows * cosVal));

    //剪掉多余边框
    int x = (dst.cols - targetSize.width) / 2;
    int y = (dst.rows - targetSize.height) / 2;
    Rect rect(x, y, targetSize.width, targetSize.height);
    dst = Mat(dst, rect);
}

void ProducerRecordImpl::run()
{
    running_ = true;

    VideoCapture cap(videoIndex_, cv::CAP_ANY);
    // cap.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
    // cap.set(cv::CAP_PROP_FRAME_HEIGHT, 720);

    if (!cap.isOpened())
    {
        cout << "Cannot open camera\n";
        abort();
    }

    Mat frame;

    while (running_)
    {
        using namespace common::time;

        static int count = 0;
        static int64_t start_time = getCurrentMilliTime();
        // LOG_DEBUG("[frame] w: {}, h:{}", frame.cols, frame.rows);

        ++count;
        if (getCurrentMilliTime() - start_time > 1000)
        {
            int diff_time = getCurrentMilliTime() - start_time;
            int fps = count * 1000 / diff_time;
            LOG_TRACE("ProducerRecordImpl FPS: {}", fps);
            count = 0;
            start_time = getCurrentMilliTime();
        }

        // 擷取影像
        TimeConsumingAnalysis anaylis;
        anaylis.addTimePoint();

        bool ret = cap.read(frame); // or cap >> frame;
        anaylis.addTimePoint("readframe");

        cv::flip(frame, frame, 1); // 0: x轴翻转  1: y轴翻转  2: 同时翻转
        anaylis.addTimePoint("flip");
        cv::Mat clone;
        rotate_arbitrarily_angle(frame, clone, 90);
        anaylis.addTimePoint("rotate");

        // LOG_DEBUG(anaylis.print());
        // LOG_DEBUG("w: {}, h: {}", clone.cols, clone.rows);
        // imshow("image", frame);
        if (!ret)
        {
            LOG_ERROR("Can't receive frame (stream end?). Exiting ...");
            break;
        }

        cv::Rect rect(0, 0, clone.cols, clone.rows);
        if (hahaUi_)
        {
            hahaUi_->addImage(clone, rect, HahaUi::Mirror);
            cv::imshow("mirrorImage", clone);
        }

        auto consumers = getRecordConsumers();
        for (auto it = consumers.begin(); it != consumers.end(); ++it)
        {
            (*it)->consumeRecord(clone, clone);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    running_ = false;
}

ProducerRecordImpl::ProducerRecordImpl(int videoIndex)
    : running_(false)
    , thread_(nullptr)
    , videoIndex_(videoIndex)
    , hahaUi_(nullptr)
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
