#include "facedetect.h"

#include <map>
#include <queue>

#include "common/logging.h"
#include "common/processinfo.h"
#include "common/time.h"
#include "hahacore.h"
#include "main_haha/include/faceRec/face_recognition_api.h"

using namespace HaHaFaceVision;

FaceDetect::FaceDetect()
    : running_(false)
    , newData_(false)
    , models_path_(common::ProcessInfo::exeDirPath() + "/model/")
    , face_handle_(0)
    , max_face_num_(6)
    , validArea_(107, 287, 866, 1446)
    , detectStatus_(true)
    , hahaCore_(nullptr)
{
}

FaceDetect::~FaceDetect()
{
    stop();
}

void FaceDetect::init()
{
    LOG_DEBUG("@@@@@ faceDete ct version: {} @@@@@", HaHaFaceVision::UBT_AIGetFRVersion_x86());

    HaHaFaceVision::FACE_CONFIG_PARAMS config_params;
    config_params.imageWidth = 2000;
    config_params.imageHeight = 2000;
    config_params.minFace = config_->detect()->faceMin;
    config_params.isColor = true;
    config_params.FaceLandmarkPose_flag = true;
    config_params.device_type = "CPU";
    HaHaFaceVision::UBT_AIFaceRecInit(models_path_, config_params, face_handle_, 3);

    taskThread_ = new std::thread(std::bind(&FaceDetect::handleTaskCallback, this));
}

void FaceDetect::start()
{
    running_ = true;
    init();
}

void FaceDetect::stop()
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

static cv::Rect calculateFaceRect(const cv::Rect rect, const int max_width, const int max_height)
{
    int width = rect.width;
    int height = rect.height;
    int x = rect.x;
    int y = rect.y;

    //  LOG_DEBUG("[old] x: {}, y: {}, width: {}, height: {}", x, y, width, height);

    int center_x = x + width / 2;
    int center_y = y + height / 2;

    int long_side = (std::max(width, height)) * 1.5;
    int half_side = long_side / 2;
    int w_left, w_right, h_up, h_down;
    w_left = w_right = h_up = h_down = half_side;

    x = center_x - half_side;
    y = center_y - half_side;
    if (x < 0)
    {
        x = 0;
        w_left = center_x;
    }
    if (y < 0)
    {
        y = 0;
        h_up = center_y;
    }

    int right = center_x + half_side;
    int down = center_y + half_side;
    if (right > max_width)
    {
        w_right = max_width - center_x;
    }
    if (down > max_height)
    {
        h_down = max_height - center_y;
    }

    // LOG_DEBUG("long_size: {}, w:{}, h: {}", long_side, x, y);
    //  LOG_DEBUG("[new] x: {}, y: {}, width: {}, height: {}", x, y, width, height);
    return cv::Rect(x, y, w_left + w_right, h_up + h_down);
}

void FaceDetect::handleTaskCallback()
{
    using namespace common::time;
    TimeConsumingAnalysis analysis;

    while (running_)
    {
        matMutex_.lock();
        if (person_mat_.empty() || detectStatus_ == false)
        {
            matMutex_.unlock();
            usleep(1 * 1000);
            continue;
        }
        cv::Mat mat = person_mat_.clone();
        bool new_data = newData_;
        matMutex_.unlock();

        //  cv::imshow("detect_exception.png", mat);

        analysis.addTimePoint("get mutex");

        const int sk_max_width = mat.cols;
        const int sk_max_height = mat.rows;
        // LOG_DEBUG("sk_max_width: {}, sk_max_height: {}", sk_max_width, sk_max_height);
        std::vector<FACE_DET_SINGLE_RESULT> faceResults;
        HaHaFaceVision::UBT_AIFaceDetection(face_handle_, mat, faceResults);
        analysis.addTimePoint("UBT_AIFaceDetection");
        const int csize = faceResults.size();
        int size = csize;
        // LOG_DEBUG("size: {}", size);
        std::map<int, std::vector<int>> areas;
        std::priority_queue<int, std::vector<int>, std::less<int>> big_heap;
        //  LOG_DEBUG("init: {}", csize);
        for (int i = 0; i < csize; ++i)
        {
            auto shapes = faceResults[i].faceShape;
            int s = 0;
            for (; s < 5; ++s)
            {
                //                if (shapes[s].x < 0 || shapes[s].x > sk_max_width || shapes[s].y < 0
                //                    || shapes[s].y > sk_max_height)
                if (!validArea_.contains(shapes[s]))
                {
                    break;
                }
            }
            if (s < 5)
            {
                LOG_TRACE("please keep whole face in screen, facefeature: {}!!", s);
                --size; // 去掉不符合条件的人脸
                continue;
            }

            int area = faceResults[i].faceRect.area();
            //LOG_DEBUG("i:{}, area: {}", i, area);
            big_heap.push(area);
            if (areas.find(area) == areas.end())
            {
                areas[area] = std::vector<int>{i};
            }
            else
            {
                areas[area].push_back(i);
            }
        }
        analysis.addTimePoint("filter face of not in area");

        std::vector<FaceDetectResult> results;
        int face_num = std::min(size, max_face_num_);
        int i = 0;
        while (i < face_num)
        {
            int val = big_heap.top();
            big_heap.pop();
            auto rects = areas[val];

            for (auto it = rects.begin(); it != rects.end() && i < face_num; ++it, ++i)
            {
                FaceDetectResult result;
                auto faceresult = faceResults[*it];
                result.bigFaceRect = calculateFaceRect(faceresult.faceRect,
                                                       sk_max_width,
                                                       sk_max_height);
                result.algorithmFaceRect = faceresult.faceRect;
                memcpy(&result.faceShape, &faceresult.faceShape, sizeof(cv::Point2f) * 5);
                results.emplace_back(result);
            }
        }
        analysis.addTimePoint("calculate big face");
        if (hahaCore_ && new_data)
        {
            //   LOG_DEBUG("update hahaCore");
            hahaCore_->setFaceInfo(mat, results);
            matMutex_.lock();
            newData_ = false;
            matMutex_.unlock();
        }
        analysis.addTimePoint("send to hahacore");

        static int face_count = 0;
        static int last_face_num = 0;
        static int last_face_num_point = 0;
        if (face_num == last_face_num)
        {
            ++face_count;
            if (face_count >= 30)
            {
                personMutex_.lock();
                person_results_.swap(results);
                personMutex_.unlock();

                if (face_num != last_face_num_point)
                {
                    emit sig_faceCountChanged(face_num, last_face_num_point);
                    last_face_num_point = face_num;
                }
                face_count = 0;
            }
        }
        else
        {
            face_count = 0;
        }

        analysis.addTimePoint("swap results");
        last_face_num = face_num;
        LOG_TRACE(analysis.print());

        mat.release();
    }
}

void FaceDetect::consumeRecord(const cv::Mat color_mat, const cv::Mat original_mat)
{
    std::unique_lock<std::mutex> guard(matMutex_);

    if (!person_mat_.empty())
    {
        person_mat_.release();
    }

    person_mat_ = color_mat.clone();
    newData_ = true;

    // LOG_DEBUG("new data into!!");
}
