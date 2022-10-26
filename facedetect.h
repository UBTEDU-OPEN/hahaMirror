#ifndef FACEDETECT_H
#define FACEDETECT_H

#include "consumer_base.h"
#include <mutex>
#include <thread>
#include <QObject>

struct FaceDetectResult
{
    cv::Rect faceRect;
    FaceDetectResult operator=(FaceDetectResult data)
    {
        this->faceRect = data.faceRect;
        return *this;
    }
};

class FaceDetect : public QObject, public ConsumerRecord
{
    Q_OBJECT
public:
    FaceDetect();
    ~FaceDetect();

    void start() override;
    void stop() override;
    void consumeRecord(const cv::Mat color_mat, const cv::Mat depth_mat) override;

    inline void setModelsPath(std::string path) { models_path_ = path; }
    std::vector<FaceDetectResult> getCurrentPersonRects()
    {
        std::unique_lock<std::mutex> guard(personMutex_);
        return person_rects_;
    }

private:
    void init();
    void handleTaskCallback();

    bool running_;
    std::vector<FaceDetectResult> person_rects_;
    cv::Mat person_mat_;
    std::string models_path_;
    int face_handle_;
    int max_face_num_;

    std::thread *taskThread_;
    std::mutex matMutex_;
    std::mutex personMutex_;
};

#endif // FACEDETECT_H
