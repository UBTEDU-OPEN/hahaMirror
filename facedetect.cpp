#include "facedetect.h"
#include "algorithm/include/faceRec/face_recognition_api.h"
#include "common/logging.h"
#include "common/time.h"

#include <map>
#include <queue>

using namespace HaHaFaceVision;

FaceDetect::FaceDetect()
    : running_(false)
    , models_path_("/home/ubt/code/Qt/hahaMirror/algorithm/model")
    , face_handle_(0)
    , max_face_num_(6)
{
}

FaceDetect::~FaceDetect()
{
    stop();
}

void FaceDetect::init()
{
    LOG_DEBUG("@@@@@ faceDetect version: {} @@@@@", HaHaFaceVision::UBT_AIGetFRVersion_x86());

    HaHaFaceVision::FACE_CONFIG_PARAMS config_params;
    config_params.imageWidth = 2000;
    config_params.imageHeight = 2000;
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
    if (taskThread_ && taskThread_->joinable())
    {
        taskThread_->join();
    }
    taskThread_ = nullptr;
}

void FaceDetect::handleTaskCallback()
{
    using namespace common::time;
    TimeConsumingAnalysis analysis;

    while (running_)
    {
        analysis.addTimePoint();
        matMutex_.lock();
        cv::Mat mat = person_mat_.clone();
        matMutex_.unlock();

        analysis.addTimePoint("get mutex");

        std::vector<FACE_DET_SINGLE_RESULT> faceResults;
        HaHaFaceVision::UBT_AIFaceDetection(face_handle_, mat, faceResults);

        std::map<int, std::vector<int>> areas;
        std::priority_queue<int, std::vector<int>, std::less<int>> big_heap;
        int size = faceResults.size();
        for (int i = 0; i < size; ++i)
        {
            int area = faceResults[i].faceRect.area();
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

        std::vector<FaceDetectResult> results;
        for (int i = 0; i < max_face_num_; ++i)
        {
            int val = big_heap.top();
            big_heap.top();
            auto rects = areas[val];

            for (auto it = rects.begin(); it != rects.end(), i < max_face_num_; ++it)
            {
                FaceDetectResult result;
                auto faceresult = faceResults[rects[*it]];
                result.faceRect = faceresult.faceRect;
                results.emplace_back(result);
            }
        }

        personMutex_.lock();
        person_rects_.swap(results);
        personMutex_.unlock();

        LOG_DEBUG("size: {}", faceResults.size());
        analysis.addTimePoint("detect faces");
        LOG_DEBUG(analysis.print());

        mat.release();
    }
}

void FaceDetect::consumeRecord(const cv::Mat color_mat, const cv::Mat depth_mat)
{
    std::unique_lock<std::mutex> guard(matMutex_);

    if (!person_mat_.empty())
    {
        person_mat_.release();
    }

    //    using namespace common::time;
    //    TimeConsumingAnalysis analysis;
    //    analysis.addTimePoint();
    //    LOG_DEBUG(analysis.print());

    person_mat_ = color_mat.clone();
}
