#ifndef FACEDETECT_H
#define FACEDETECT_H

#include <mutex>
#include <thread>
#include <QObject>

#include "consumer_base.h"

struct FaceDetectResult
{
    cv::Rect bigFaceRect;
    cv::Rect algorithmFaceRect;
    cv::Point2f faceShape[5];
    FaceDetectResult operator=(FaceDetectResult data)
    {
        this->bigFaceRect = data.bigFaceRect;
        this->algorithmFaceRect = data.algorithmFaceRect;
        memcpy(&data.faceShape, &this->faceShape, sizeof(cv::Point2f) * 5);
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
    void consumeRecord(const cv::Mat color_mat, const cv::Mat original_mat) override;

    inline void setModelsPath(std::string path) { models_path_ = path; }
    std::vector<FaceDetectResult> getCurrentPersonResults()
    {
        std::unique_lock<std::mutex> guard(personMutex_);
        return person_results_;
    }

signals:
    void sig_faceCountChanged(int cur, int last);

private:
    void init();
    void handleTaskCallback();

    bool running_;
    std::vector<FaceDetectResult> person_results_;
    cv::Mat person_mat_;
    std::string models_path_;
    int face_handle_;
    int max_face_num_;

    std::thread *taskThread_;
    std::mutex matMutex_;
    std::mutex personMutex_;
};

#endif // FACEDETECT_H
