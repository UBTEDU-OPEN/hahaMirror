#ifndef HAHACORE_H
#define HAHACORE_H

#include "config.h"
#include "consumer_base.h"

#include <QObject>
#include <QTimer>
#include <mutex>
#include <opencv2/opencv.hpp>
#include <thread>

enum HahaEffect
{
    Eyes,
    Mouth,
    Face,
    None
};

class FaceDetect;
struct FaceDetectResult;

class Hahacore : public QObject, public ConsumerRecord
{
    Q_OBJECT
public:
    Hahacore();
    ~Hahacore();

    void start() override;
    void stop() override;
    void consumeRecord(const cv::Mat color_mat,
                       const cv::Mat original_mat) override;

    inline void setHahaEffect(HahaEffect effect)
    {
        std::lock_guard<std::mutex> guard(effectMutex_);
        effect_ = effect;
    }

    void setFaceDetectObject(FaceDetect* faceDetectObj)
    {
        faceDetectObj_ = faceDetectObj;
    }

    void setConfig(config::Config* config)
    {
        config_ = config;
        beauty_ = config_->haha()->beauty;
    }
    void setFaceInfo(cv::Mat& mat, std::vector<FaceDetectResult> result);

signals:
    void sig_sendHahaMat(cv::Mat);

private slots:
    void slot_timeout();

private:
    void init();

    void handleTaskCallback();

    bool running_;
    HahaEffect effect_;
    std::thread* taskThread_;
    std::mutex sourceMatMutex_;
    std::mutex effectMutex_;
    cv::Mat sourceMat_;

    std::mutex hahaMatMutex_;
    cv::Mat hahaMat_;
    FaceDetect* faceDetectObj_;
    config::Config* config_;
    std::vector<FaceDetectResult> faceDetectResult_;
    QTimer timer_;
    int interval_time_;
    int beauty_;
};

#endif // HAHACORE_H
