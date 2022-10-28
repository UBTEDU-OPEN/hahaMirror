#ifndef HAHACORE_H
#define HAHACORE_H

#include "consumer_base.h"

#include <mutex>
#include <opencv2/opencv.hpp>
#include <thread>
#include <QObject>

enum HahaEffect
{
    Eyes,
    Mouth,
    Face,
    None
};

class FaceDetect;

class Hahacore : public QObject, public ConsumerRecord
{
    Q_OBJECT
public:
    Hahacore();
    ~Hahacore();

    void start() override;
    void stop() override;
    void consumeRecord(const cv::Mat color_mat, const cv::Mat original_mat) override;

    inline void setHahaEffect(HahaEffect effect)
    {
        std::lock_guard<std::mutex> guard(effectMutex_);
        effect_ = effect;
    }

    void setFaceDetectObject(FaceDetect *faceDetectObj) { faceDetectObj_ = faceDetectObj; }

signals:
    void sig_sendHahaMat(cv::Mat);

private:
    void init();

    void handleTaskCallback();

    bool running_;
    HahaEffect effect_;
    std::thread *taskThread_;
    std::mutex sourceMatMutex_;
    std::mutex effectMutex_;
    cv::Mat sourceMat_;

    std::mutex hahaMatMutex_;
    cv::Mat hahaMat_;
    FaceDetect *faceDetectObj_;
};

#endif // HAHACORE_H
